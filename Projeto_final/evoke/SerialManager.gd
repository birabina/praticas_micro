extends Node

signal resposta_da_stm32(comando)

var arquivo_serial: FileAccess
# Altere para "/dev/ttyUSB0" se sua placa usar chip CH340/CP2102
var porta_serial = "/dev/ttyACM0" 

func _ready():
	if FileAccess.file_exists(porta_serial):
		arquivo_serial = FileAccess.open(porta_serial, FileAccess.READ_WRITE)
		print("Sucesso: Conectado à STM32 em: ", porta_serial)
	else:
		print("Aviso: Porta ", porta_serial, " não encontrada. Rodando em modo de simulação.")

func _process(_delta):
	if arquivo_serial and arquivo_serial.get_length() > 0:
		var linha = arquivo_serial.get_line().strip_edges()
		if linha != "":
			emit_signal("resposta_da_stm32", linha)

func enviar_comando(texto: String):
	if arquivo_serial:
		arquivo_serial.store_line(texto)
		print("PC enviou: ", texto)
	else:
		print("Modo Simulação - Comando ignorado: ", texto)
