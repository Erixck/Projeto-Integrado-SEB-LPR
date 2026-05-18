### Alunos:
Erick Guilherme de Carlos Rosa
Gustavo Vilas Boas Batista

### Detalhamento das Fases:
1.  **Fase 0 (Aguardando Inicialização):** O sistema permanece em espera ativa. Ao pressionar PA9 o HAL_GetTick() é capturado como semente de entropia para a função srand(), gerando uma senha aleatória única de 4 dígitos entre 1000 e 9999.
2.  **Fase 1 (Validação de Senha):** O display exibe os caracteres sob edição, a quantidade de dígitos inseridos e a senha correta (solicitação específica de projeto). Caso o usuário erre a credencial por 3 vezes consecutivas, o sistema entra em estado de proteção por 10 segundos, utilizando temporização.
3.  **Fase 2 (Configuração de Limite):** Define a lotação máxima permitida para o laboratório naquela sessão. O botão PA10 faz o incremento unitário e o PA11 salva a configuração.
4.  **Fase 3 (Controle de Aula):** Gerencia a dinâmica da sala:
    * **Entrada de Aluno:** Exige uma pré-autenticação simulada (PA12). Uma vez validada a digital, o botão (PA9) libera a entrada do estudante, validando se a sala não atingiu o limite configurado na Fase 2.
    * **Gestão de Concorrência do Banheiro:** Controla as saídas (PA10) e retornos (PA11). O sistema limita a ocupação do banheiro a **no máximo 3 alunos simultâneos**. Tentativas de saídas extras são bloqueadas com avisos.
5.  **Fase 4 (Relatório Final):** A qualquer momento da aula, o professor pode encerrar a sessão pressionando PA9 + PA12. O display limpa a tela de monitoramento e mostra o relatório.

---

## Decisões de Projeto

1.  **Tratamento de Debounce e Concorrência de Botões:**
    Botões mecânicos sofrem com o efeito de *bouncing* (ruído elétrico de oscilação). Para diminuir isso sem travar a execução do programa principal, foram implementados delays de estabilização (`HAL_Delay(50)` a `HAL_Delay(60)`).
2.  **Segurança de Estado Não-Bloqueante:**
    O bloqueio temporizado de 10 segundos por erro de senha foi desenvolvido utilizando a leitura do *Systick Timer* (`HAL_GetTick() - tempo_bloqueio`). Isso impede o uso de delays absolutos longos que congelariam a atualização da contagem regressiva no display.
---

## Guia de Uso (Passo a Passo)

Siga estas etapas para operar o sistema após carregar o código na placa:

### 1. Inicialização e Geração de Senha
Ao ligar, o display mostrará "SISTEMA PRONTO".
* **Ação:** Pressione **PA9**.
* **O que acontece:** O sistema gera uma semente aleatória e cria uma senha de 4 dígitos. A aula entra em fase de autenticação.

### 2. Autenticação do Professor
Nesta etapa, você deve inserir a senha gerada (que aparece no display para fins de teste/exibição).
* **PA9:** Altera o dígito atual (0 a 9).
* **PA10:** Confirma o dígito e pula para o próximo.
* **Segurança:** Se errar a senha 3 vezes, o sistema trava por 10 segundos para segurança.

### 3. Configuração de Limite
Antes da aula começar, defina quantos alunos a sala suporta.
* **PA10:** Incrementa o limite de alunos.
* **PA11:** Confirma o valor e inicia a aula oficialmente.

### 4. Durante a Aula (Controle de Fluxo)
Com a aula em curso, o display monitora o status em tempo real:
* **Entrada de Aluno:** Primeiro pressione **PA12** (Simula leitura de biometria). Se a digital for aceita, pressione **PA9** (Simula a catraca). O sistema impede a entrada se o limite da sala for atingido.
* **Saída para Banheiro:** Pressione **PA10**. O sistema permite no máximo 3 alunos fora ao mesmo tempo.
* **Retorno do Banheiro:** Pressione **PA11** para registrar que o aluno voltou.

### 5. Encerramento e Relatório Final
Para finalizar a aula e ver as estatísticas:
* **Ação:** Mantenha pressionados **PA9 e PA12 simultaneamente**.
* **Resultado:** O sistema trava e exibe o **Relatório Final** com:
    * Total de presenças registradas.
    * Total de saídas para o banheiro.

Link vídeo explicativo: https://www.youtube.com/watch?v=VTWxst2lT08
