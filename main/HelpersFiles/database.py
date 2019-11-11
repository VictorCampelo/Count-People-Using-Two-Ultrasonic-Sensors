# Importamos a biblioteca:
import pymysql

class BancoDeDados:
    def conexaoBD(self): #retorna uma conexao com o BD
        #sensores temperatura DisnelLab2019
        return pymysql.connect(db='sensores_2', user='temperatura', passwd='DisnelLab2019')


    def buscaNo(self,id):
        # Abrimos uma conexão com o banco de dados:
        c = self.conexaoBD()

        # Cria um cursor:
        cursor = c.cursor()

        # Executa o comando:
        cmd = "SELECT descricao,tipo FROM nodes WHERE id_node = '"+id+"'"
        cursor.execute(cmd)

        # Recupera o resultado:
        resultado = cursor.fetchall()

        # Por padrão, não é efetuado commit automaticamente. Você deve commitar para salvar suas alterações.
        c.commit()

        #fecha conexao
        c.close()

        return  not (resultado == ())


    def insereDados_Sensores(self,id,data,horario,temp,corrente,umidade): # insere dados na tabela registros
        # Abrimos uma conexão com o banco de dados:
        c = self.conexaoBD()

        # Cria um cursor:
        cursor = c.cursor()

        # Executa o comando:
        cmd = "INSERT INTO registros_sensores (id_node,dia,horario,temperatura,corrente,umidade) VALUES ('"+id+"','"+data+"','"+horario+"','"+temp+"','"+corrente+"','"+umidade+"')"
        cursor.execute(cmd)

        # Efetua um commit no banco de dados.
        # Por padrão, não é efetuado commit automaticamente. Você deve commitar para salvar
        # suas alterações.
        c.commit()

        # fecha conexao
        c.close()



    def insereDados_Controle(self,id,data,horario,temp,estado): # insere dados na tabela registros
        # Abrimos uma conexão com o banco de dados:
        #print(ip, data, horario, temp, estado)
        c = self.conexaoBD()

        # Cria um cursor:
        cursor = c.cursor()

        # Executa o comando:
        #import pdb; pdb.set_trace()

        cmd = "INSERT INTO registros_controle (id_node,dia,horario,temperatura,estado) VALUES ('"+id+"','"+data+"','"+horario+"','"+temp+"','"+estado+"')"
        cursor.execute(cmd)

        # Efetua um commit no banco de dados.
        # Por padrão, não é efetuado commit automaticamente. Você deve commitar para salvar
        # suas alterações.
        c.commit()

        # fecha conexao
        c.close()

    def alteraDados_Controle(self,ip,data,horario,horario2,temp): # insere dados na tabela registros
        # Abrimos uma conexão com o banco de dados:
        c = self.conexaoBD()

        # Cria um cursor:
        cursor = c.cursor()

        # Executa o comando:
        #import pdb; pdb.set_trace()
        sql = "UPDATE registros_controle SET horario =%s,temperatura=%s  WHERE id_node = %s AND dia = %s AND horario =%s"
        val = [horario2,temp,ip,data,horario]

        #cmd = "UPDATE registros_sensores SET ip_Node = '"+ip+"',dia ='" + ""+"',horario ='" + "'"

        cursor.execute(sql,val)

        # Efetua um commit no banco de dados.
        # Por padrão, não é efetuado commit automaticamente. Você deve commitar para salvar
        # suas alterações.
        c.commit()

        # fecha conexao
        c.close()


    def insereNodes(self,id,tipo):
        # Abrimos uma conexão com o banco de dados:
        c = self.conexaoBD()

        # Cria um cursor:
        cursor = c.cursor()

        # Executa o comando:
        cmd = "INSERT INTO nodes (id_node,tipo,id_amb) VALUES ('" + id +"','"+ str(tipo) +"','"+ str(1) +"')"
        cursor.execute(cmd)

        # Efetua um commit no banco de dados.
        # Por padrão, não é efetuado commit automaticamente. Você deve commitar para salvar
        # suas alterações.
        c.commit()

        # fecha conexao
        c.close()


    def clear_All(self):
        # Abrimos uma conexão com o banco de dados:
        c = self.conexaoBD()

        # Cria um cursor:
        cursor = c.cursor()

        cmd = "DElETE  FROM registros_controle "
        cursor.execute(cmd)

        cmd = "DElETE  FROM registros_sensores "
        cursor.execute(cmd)

        # Executa o comando:
        cmd = "DElETE  FROM nodes "
        cursor.execute(cmd)

        # Efetua um commit no banco de dados.
        # Por padrão, não é efetuado commit automaticamente. Você deve commitar para salvar
        # suas alterações.
        c.commit()

        # fecha conexao
        c.close()

#a = BancoDeDados()
#a.clear_All()

