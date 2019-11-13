import socket
from BD_IOT import DatabaseIOT
from datetime import datetime
from datetime import timedelta
import numpy as np

def dateTime():
    '''
    :return: Retorna a data e a hora do PC no momento
    '''
    timestamp = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
    print(type(timestamp))
    return timestamp


def receive_msg(con):
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


def send_data_to_BD(reg):
    '''
    :param reg: os dados que vão ser inseridos no BD
    :return:
    '''
    bd = DatabaseIOT()
    if bd.seachNode(reg[0]):
        #0 = id, 1 = numberPeople, 2 = dataTime
        bd.insertDataSensor(reg[0], reg[1], reg[2])
    else:
        bd.insertNode(reg[0])
        #0 = id, 1 = numberPeople, 2 = dataTime
        bd.insertDataSensor(reg[0], reg[1], reg[2])   

def controller(msg):
    """
        Vai ser responsavel por filtrar as mensagens que chegam
        id - numeroDePessoas - timestamp
    """
    msg.pop(-1)  # remove a ultima palavra da string que é uma msg de controle
    print(dateTime())
    msg.append(str(dateTime()))
    return msg

def main():
    global con
    HOST = '10.94.15.69'  # Endereco IP do Servidor
    PORT = 9998 # Porta que o Servidor está

    sensores = {}
    controle = ''


    tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    orig = (HOST, PORT)

    tcp.bind(orig)
    tcp.listen(10)

    print("Server ON")

    while True:
        try:
            try:
                con, client = tcp.accept()
                n_PeopleIno = receive_msg(con)
            except Exception as err:
                try:
                    con.close()
                except Exception as err:
                    print("Err: Dont Closed conection")

                h = dateTime()
                e = "  Err: {0} in day try 1".format(err)+ str(h) +"\n"
                arq = open('log.txt', 'a+')
                arq.write(e)
                arq.close()
                continue

            if not n_PeopleIno: continue

            msg = controller(n_PeopleIno)
            print(msg[0])
            send_data_to_BD(msg)

        except Exception as err:
            h = dateTime()
            e = "  Err: {0} in day try 2".format(err)+ str(h) +"\n"
            arq = open('log.txt', 'a+')
            arq.write(e)
            arq.close()
        continue

main()