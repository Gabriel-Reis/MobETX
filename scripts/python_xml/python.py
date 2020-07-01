import xml.etree.ElementTree as ET
import logging
logging.basicConfig(level=logging.INFO)
# debug(), info(), warning(), error(), and critical()
import shutil

# BASICS
# print ("Hello World")
# variavel = 5+5;
# print (variavel)
# print (50+10)

#SHUTIL
# shutil.which("ls")
# shutil.copystat("file_path", "path_to_file_where_you_want_to_copy")
# shutil.move("file_path", "path_to_where_you_want_to_move_file")
# shutil.rmtree("dir_path")    ***********REMOVE TODO O DIRETORIO RECURSIVAMENTE******************

def ImprimeTudo(root,string):
	logging.debug('Imprime Init')
	print("\n---------------",string,"---------------")
	for child in root.findall("CD"):
		for title in child.findall("TITLE"):
			print(title.text)
	logging.debug('Imprime End')

def RemoveEntrada(root,childName,text):
	logging.debug('RemEntrace Init')
	for child in root.findall("CD"):
		for subelem in child.findall(childName):
			if subelem.text == text:
				root.remove(child)
	logging.debug('RemEntrace End')

def AdicionaElemento(root,array):
	logging.debug('AddElem Init')
	a = ET.Element('CD')
	child = ET.SubElement(a, 'TITLE')
	child.text = array[0]
	child = ET.SubElement(a, 'ARTIST')
	child.text = array[1]
	child = ET.SubElement(a, 'COUNTRY')
	child.text = array[2]
	child = ET.SubElement(a, 'YEAR')
	child.text = array[3]
	root.append(a);
	logging.debug('AddElem End')

arquivo = "teste.xml"
tree =  ET.parse(arquivo)
root = tree.getroot()

logging.info('--CDs DO LINKIN PARK--')
# PESQUISA CDS DO LINK PARK
for child in root.findall("CD"):
	for artist in child.findall("ARTIST"):
		if artist.text == "Linkin Park":
			print(child.find("TITLE").text); 

# ADICIONAR NOVA ENTRADA
entrada = ["NEW TITLE","NEW ARTIST","NEW COUNTRY","2020"]
logging.debug('Entrada criada')
AdicionaElemento(root,entrada)
ImprimeTudo(root,"Tudo")
RemoveEntrada(root,"ARTIST","NEW ARTIST")
ImprimeTudo(root,"Tudo")


logging.debug('Escreve no arquivo')
tree.write('teste.xml')
logging.debug('END')