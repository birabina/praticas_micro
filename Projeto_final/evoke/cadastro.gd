extends Control

# Aqui estamos dizendo para o código onde encontrar cada elemento na tela
@onready var input_nick = $InputNick
@onready var botao_confirmar = $BotaoConfirmar
@onready var painel_voz = $PainelVoz
@onready var texto_instrucao = $PainelVoz/TextoInstrucao
@onready var barra_status = $PainelVoz/BarraStatus

func _ready():
	# Quando o jogo começa, o painel de voz fica escondido por segurança
	painel_voz.visible = false

# Esta função será chamada quando o botão for clicado
func _on_botao_confirmar_pressed():
	var nick = input_nick.text
	if nick == "":
		return
		
	input_nick.editable = false
	botao_confirmar.disabled = true
	painel_voz.visible = true
	
	# PASSO 1 DO NOSSO PLANO:
	texto_instrucao.text = "Prepare-se...\nQuando a barra piscar em verde, diga em voz alta: PULAR"
	
	# MUDANÇA AQUI: Avisamos o gerenciador para enviar "T0" (Treinar comando 0) para a STM32!
	SerialManager.enviar_comando("T0") 
	
