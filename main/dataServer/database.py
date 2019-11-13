# Importamos a biblioteca:
import pymysql

class Database:
    def conexaoBD(self): #retorna uma conexao com o BD
        #sensores temperatura DisnelLab2019
        #return pymysql.connect(db='iot_count_people', user='ViFaRi', passwd='topRedCom@2019')
        #return pymysql.connect(db='iot_count_people', user='root', passwd='root')
        return pymysql.connect(db='iot_count_people', user='temperatura', passwd='DisnelLab2019')


    def seachNode(self,id):
        # Abrimos uma conexão com o banco de dados:
        c = self.conexaoBD()

        # Cria um cursor:
        cursor = c.cursor()

        # Executa o comando:
        cmd = "SELECT * FROM node WHERE idnode = '"+id+"'"
        cursor.execute(cmd)

        # Recupera o result:
        result = cursor.fetchall()

        # Por padrão, não é efetuado commit automaticamente. Você deve commitar para salvar suas alterações.
        c.commit()

        #fecha conexao
        c.close()

        return  not (result == ())


    def insertDataSensor(self,id,numPeople, datetime): # insere dados na tabela registros
        # Abrimos uma conexão com o banco de dados:
        c = self.conexaoBD()

        # Cria um cursor:
        cursor = c.cursor()

        # Executa o comando:
        cmd = "INSERT INTO countReg (numPeople, dtime, node_idnode) VALUES ('"+numPeople+"','"+datetime+"','"+id+"')"
        cursor.execute(cmd)

        # Efetua um commit no banco de dados.
        # Por padrão, não é efetuado commit automaticamente. Você deve commitar para salvar
        # suas alterações.
        c.commit()

        # fecha conexao
        c.close()

    def insertNode(self,id):
        # Abrimos uma conexão com o banco de dados:
        c = self.conexaoBD()

        # Cria um cursor:
        cursor = c.cursor()

        # Executa o comando:
        cmd = "INSERT INTO nodes (id_node) VALUES ('" + id +"')"
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

