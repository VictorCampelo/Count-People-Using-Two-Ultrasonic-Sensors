import socket
from BD_2 import BancoDeDados
from datetime import datetime
from datetime import timedelta
import numpy as np


def dataHora():
    '''
    :return: Retorna a data e a hora do PC no momento
    '''
    data_e_hora_atuais = datetime.now()
    return data_e_hora_atuais.strftime("%d/%m/%Y %H:%M:%S").split()


def recebe_msg(con):
    '''
    :param con: conexao socket aberta no momento
    :return: mensagem recebida
    '''
    m = ''
    msg = con.recv(1024)
    m = str(msg, 'cp437').split()
    while not (b'fim' in msg):
        msg = con.recv(1024)
        if not msg : return
        m = str(msg, 'cp437').split() #decodificando a msg

    con.close()

    return m


def envia_pro_BD(tipo,registros):
    '''
    :param tipo: tipo de registro - 1: Controle , 2: Sensores
    :param registros: os dados que vão ser inseridos no BD
    :return:
    '''
    bd = BancoDeDados()
    if(tipo):
        aux = registros[5]
        ultimo = aux[1].split(':')
        #import pdb; pdb.set_trace()
        ultimo = timedelta(days = 0, hours = int(ultimo[0]), minutes = int(ultimo[1]),seconds=int(ultimo[2]))
        novo = registros[4].split(':')
        novo = timedelta(days = 0, hours = int(novo[0]), minutes = int(novo[1]),seconds=int(novo[2]))
        result = novo - ultimo

        if np.abs(result.total_seconds()) < 20:
            bd.alteraDados_Controle(registros[0], aux[0], aux[1], registros[4], registros[1])
        else:
            bd.insereDados_Controle(registros[0],registros[3],registros[4],registros[1],registros[2])

    else:
        for i in registros:
            if len(registros[i]) == 6: #condicao para mandar pro servidor
                med_temp = 0
                med_umi = 0
                sum_corrente = 0
                #import pdb; pdb.set_trace()


                for j in registros[i]:
                    if not (j[1] or j[2] or j[3]):
                        return
                    med_temp += float(j[1])
                    med_umi += float(j[3])
                    sum_corrente += float(j[2])

                med_temp = round(med_temp /6)
                med_umi = round(med_umi /6)

                if bd.buscaNo(j[0]):
                    bd.insereDados_Sensores(j[0], j[4], j[5], str(med_temp), str(sum_corrente), str(med_umi))
                else:
                    bd.insereNodes(j[0], tipo)
                    bd.insereDados_Sensores(j[0], j[4], j[5], str(med_temp), str(sum_corrente), str(med_umi))


                registros[i] = []



def controlador(msg):
    """
        Vai ser responsavel por filtrar as mensagens que chegam
    """
    msg.pop(-1)  # remove a ultima palavra da string que é uma msg de controle
    t = msg.pop(0)  # remove a primeira palavra da string que é o codigo do tipo de msg

    if t == '0': #Nós que estão coletando temperatura/corrente
        data, horario = dataHora()  # horario
        msg.append(data)
        msg.append(horario)
        #print(msg)

    elif t == '1':#Nó controle do Ar
        data, horario = dataHora()  # horario
        msg.append(data)
        msg.append(horario)

    return int(t),msg

#-------------------------main---------------------------------
conectados = [] # lista de nodes que estão conectados

HOST = '10.94.15.69'  # Endereco IP do Servidor
PORT = 9999
# Porta que o Servidor está

sensores = {}
controle = ''
tempo_controle = dataHora()

tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

orig = (HOST, PORT)

tcp.bind(orig)
tcp.listen(7)

print("Servidor On")
print(dataHora())

while True:
    try:
        try:
            con, cliente = tcp.accept()
            a = recebe_msg(con)
        except Exception as err:
            try:
                con.close()
            except Exception as err:
                print("Não conseguiu fechar a conexao")
            d, h = dataHora()
            e = "  Error: {0} no dia".format(err) + d + " as " + h
            arq = open('log.txt', 'a+')
            arq.write(e)
            arq.close()
            continue

        if not a: continue

        t,msg = controlador(a) #   t: tipo de msg       msg: a mensagem

        print(msg[0])
        if t == 1:
            msg.append(tempo_controle)
            controle = cliente
            envia_pro_BD(t, msg)
            tempo_controle = (msg[3] + " " + msg[4]).split()


        if cliente[0] in sensores and t == 0:  # se ip já é conhecido
            sensores[cliente[0]].append(msg)
            #if len(sensores[cliente[0]]) == 6:
            #print("Sensor" , msg)
            envia_pro_BD(t, sensores)
        elif t == 0:
            sensores.update({cliente[0]: []})
            sensores[cliente[0]].append(msg)
            envia_pro_BD(t, sensores)


    except Exception as err:
        d,h = dataHora()
        e = "  Error: {0} no dia".format(err) + d + " as " +h +"\n"
        arq = open('log.txt', 'a+')
        arq.write(e)
        arq.close()
        continue




