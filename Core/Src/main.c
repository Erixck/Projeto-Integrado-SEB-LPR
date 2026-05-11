/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st7735\st7735.h"
#include "stdlib.h"
#include "time.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
GPIO_PinState estado_anterior_botao1 = GPIO_PIN_SET;
GPIO_PinState estado_anterior_botao2 = GPIO_PIN_SET;
GPIO_PinState estado_anterior_botao3 = GPIO_PIN_SET;
GPIO_PinState estado_anterior_botao4 = GPIO_PIN_SET;

int aula_iniciada = 0;
int sistema_autenticado = 0;
int alunos_presentes = 0;
int tentativas_erradas = 0;
int sistema_bloqueado = 0;
long tempo_bloqueio = 0;
int matricula = 0;
int limite_maximo_alunos = 10;
int config_finalizada = 0;
int senha_inserida = 0;
int senha_gerada = 0;
int digito_atual = 0;
int posicao_digito = 0;
int alunos_fora = 0;
int total_entrada = 0;
int total_saida = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
void exibir_tela_inicial(void);
void gerar_senha_aleatoria(void);
int verificar_senha(int senha_gerada, int senha_inserida);
void exibir_senha_tela(int senha_gerada);
void inserir_senha(void);
void atualizar_tela_inserir(int posicao_digito, int digito_atual, int senha_inserida);
void gerenciar_configuracao(void);
void atualizar_tela_configuracao(void);
void atualizar_tela_principal(void);
void gerenciar_aula(void);
void exibir_relatorio_final(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  ST7735_Init();
  srand(HAL_GetTick());
  exibir_tela_inicial();
  gerar_senha_aleatoria();
  while (aula_iniciada == 0) {
      if (HAL_GPIO_ReadPin(BOTAO1_GPIO_Port, BOTAO1_Pin) == GPIO_PIN_RESET) {
          aula_iniciada = 1;
          HAL_Delay(200);
      }
  }
  exibir_senha_tela(senha_gerada);
  HAL_Delay(2000);
  atualizar_tela_inserir(posicao_digito, digito_atual, senha_inserida);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      if (sistema_autenticado == 0) {
          inserir_senha();
      }

      if (sistema_autenticado == 1 && config_finalizada == 0) {
          gerenciar_configuracao();
      }

      if (sistema_autenticado == 1 && config_finalizada == 1) {
          gerenciar_aula();

          if (HAL_GPIO_ReadPin(BOTAO1_GPIO_Port, BOTAO1_Pin) == GPIO_PIN_RESET &&
              HAL_GPIO_ReadPin(BOTAO4_GPIO_Port, BOTAO4_Pin) == GPIO_PIN_RESET) {

              exibir_relatorio_final();
              while(1);
          }
      }
	      /* USER CODE END WHILE */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ST7735_DC_Pin|ST7735_RES_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : ST7735_CS_Pin */
  GPIO_InitStruct.Pin = ST7735_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ST7735_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ST7735_DC_Pin ST7735_RES_Pin */
  GPIO_InitStruct.Pin = ST7735_DC_Pin|ST7735_RES_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BOTAO1_Pin BOTAO2_Pin BOTAO3_Pin BOTAO4_Pin */
  GPIO_InitStruct.Pin = BOTAO1_Pin|BOTAO2_Pin|BOTAO3_Pin|BOTAO4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void exibir_tela_inicial(void);

void exibir_tela_inicial(void) {
    ST7735_FillScreen(BLACK);
    ST7735_WriteString(10, 40, "BEM-VINDO!", Font_11x18, WHITE, BLACK);
    ST7735_WriteString(10, 70, "Pressione B1", Font_7x10, YELLOW, BLACK);
    ST7735_WriteString(10, 85, "para iniciar", Font_7x10, YELLOW, BLACK);
}

void gerar_senha_aleatoria(){
	senha_gerada = (rand() % 9000) + 1000;
}

void exibir_senha_tela(int senha_gerada){
	ST7735_FillScreen(BLACK);
	char buffer[20];
	sprintf(buffer, "SENHA: %04d", senha_gerada);
	ST7735_WriteString(10, 40, buffer, Font_11x18, YELLOW, BLACK);
	HAL_Delay(200);
}

void inserir_senha(void){

    if (sistema_bloqueado) {
        if (HAL_GetTick() - tempo_bloqueio >= 10000) {
            sistema_bloqueado = 0;
            tentativas_erradas = 0;
            atualizar_tela_inserir(posicao_digito, digito_atual, senha_inserida);
        } else {
            return;
        }
    }

    GPIO_PinState atual_b2 = HAL_GPIO_ReadPin(BOTAO2_GPIO_Port, BOTAO2_Pin);
    if (atual_b2 == GPIO_PIN_RESET && estado_anterior_botao2 == GPIO_PIN_SET){
        digito_atual = (digito_atual + 1) % 10;
        atualizar_tela_inserir(posicao_digito, digito_atual, senha_inserida);
    }
    estado_anterior_botao2 = atual_b2;

    GPIO_PinState atual_b3 = HAL_GPIO_ReadPin(BOTAO3_GPIO_Port, BOTAO3_Pin);
    if (atual_b3 == GPIO_PIN_RESET && estado_anterior_botao3 == GPIO_PIN_SET){
        if (posicao_digito == 0) senha_inserida += (digito_atual * 1000);
        else if (posicao_digito == 1) senha_inserida += (digito_atual * 100);
        else if (posicao_digito == 2) senha_inserida += (digito_atual * 10);
        else if (posicao_digito == 3) senha_inserida += digito_atual;

        if (posicao_digito == 3) {
            sistema_autenticado = verificar_senha(senha_gerada, senha_inserida);

            if(sistema_autenticado == 1) {
                ST7735_FillScreen(BLACK);
                ST7735_WriteString(10, 50, "ACESSO LIBERADO!", Font_7x10, GREEN, BLACK);
                HAL_Delay(1500);
            } else {
                tentativas_erradas++;
                ST7735_FillScreen(BLACK);

                if (tentativas_erradas >= 3) {
                    sistema_bloqueado = 1;
                    tempo_bloqueio = HAL_GetTick();
                    ST7735_WriteString(10, 50, "SISTEMA BLOQUEADO", Font_7x10, RED, BLACK);
                    ST7735_WriteString(10, 70, "Aguarde 10s...", Font_7x10, RED, BLACK);
                } else {
                    ST7735_WriteString(10, 50, "SENHA ERRADA!", Font_7x10, RED, BLACK);
                }
                HAL_Delay(1500);

                posicao_digito = -1;
                senha_inserida = 0;
            }
        }

        posicao_digito++;
        digito_atual = 0;

        if (sistema_autenticado == 0 && sistema_bloqueado == 0) {
            atualizar_tela_inserir(posicao_digito, digito_atual, senha_inserida);
        }
    }
    estado_anterior_botao3 = atual_b3;
}

void atualizar_tela_inserir(int posicao_digito, int digito_atual, int senha_inserida){
	char texto_formatado[20];

	ST7735_FillScreen(BLACK);

	ST7735_WriteString(10, 10, "DIGITE A SENHA", Font_7x10, WHITE, BLACK);

	if (posicao_digito == 0)
	        sprintf(texto_formatado, "> %d < _ _ _", digito_atual);
	    else if (posicao_digito == 1)
	        sprintf(texto_formatado, "%d > %d < _ _", (senha_inserida/1000), digito_atual);
	    else if (posicao_digito == 2)
	        sprintf(texto_formatado, "%d %d > %d < _", (senha_inserida/1000), (senha_inserida%1000)/100, digito_atual);
	    else if (posicao_digito == 3)
	        sprintf(texto_formatado, "%d %d %d > %d <", (senha_inserida/1000), (senha_inserida%1000)/100, (senha_inserida%100)/10, digito_atual);

	ST7735_WriteString(10, 50, texto_formatado, Font_11x18, YELLOW, BLACK);
	ST7735_WriteString(10, 100, "B1:+  B2:OK", Font_7x10, GREEN, BLACK);
}

int verificar_senha(int senha_gerada, int senha_inserida){
	if (senha_gerada == senha_inserida){
		return 1;
	} else {
		return 0;
	}
}

void gerenciar_configuracao(void){
	GPIO_PinState atual_b2 = HAL_GPIO_ReadPin(BOTAO2_GPIO_Port, BOTAO2_Pin);

	if (atual_b2 == GPIO_PIN_RESET && estado_anterior_botao2 == GPIO_PIN_SET){
		limite_maximo_alunos++;
		if (limite_maximo_alunos > 40){
			limite_maximo_alunos = 1;
		}

		atualizar_tela_configuracao();
		HAL_Delay(50);
	}

	estado_anterior_botao2 = atual_b2;

	GPIO_PinState atual_b3 = HAL_GPIO_ReadPin(BOTAO3_GPIO_Port, BOTAO3_Pin);
	if (atual_b3 == GPIO_PIN_RESET && estado_anterior_botao3 == GPIO_PIN_SET){
		config_finalizada = 1;

		ST7735_FillScreen(BLACK);
		ST7735_WriteString(10, 50, "TURMA CONFIGURADA!", Font_7x10, GREEN, BLACK);
		HAL_Delay(1000);

		atualizar_tela_principal();
	}
	estado_anterior_botao3 = atual_b3;
}

void atualizar_tela_configuracao(){
	char buffer[20];

	ST7735_FillScreen(BLACK);
	ST7735_WriteString(5, 10, "CONFIG. DA TURMA", Font_7x10, WHITE, BLACK);
	ST7735_WriteString(5, 40, "Qtd Max Alunos:", Font_7x10, WHITE, BLACK);

	sprintf(buffer, "%02d", limite_maximo_alunos);

	ST7735_WriteString(50, 70, buffer, Font_11x18, CYAN, BLACK);

	ST7735_WriteString(5, 110, "B2 [+]  B3 [OK]", Font_7x10, YELLOW, BLACK);
}

void atualizar_tela_principal(void){
    char buffer[32];
    ST7735_FillScreen(BLACK);

    sprintf(buffer, "Alunos: %02d/%02d", alunos_presentes, limite_maximo_alunos);
    ST7735_WriteString(5, 20, buffer, Font_11x18, WHITE, BLACK);


    ST7735_FillRectangle(5, 45, 118, 10, WHITE);
    int largura_barra = (alunos_presentes * 114) / limite_maximo_alunos;
    if (largura_barra > 0) {
        ST7735_FillRectangle(7, 47, largura_barra, 6, BLUE);
    }


    sprintf(buffer, "No Banheiro: %d/3", alunos_fora);
    ST7735_WriteString(5, 65, buffer, Font_7x10, WHITE, BLACK);

    if (alunos_fora >= 3) {
        ST7735_WriteString(5, 80, "STATUS: OCUPADO", Font_7x10, RED, BLACK);
    } else {
        ST7735_WriteString(5, 80, "STATUS: LIVRE  ", Font_7x10, GREEN, BLACK);
    }

    ST7735_WriteString(5, 115, "B1:Leitor B2:Out B3:In", Font_7x10, WHITE, BLACK);
}
void gerenciar_aula(void) {
    GPIO_PinState atual_b1 = HAL_GPIO_ReadPin(BOTAO1_GPIO_Port, BOTAO1_Pin);

    if (atual_b1 == GPIO_PIN_RESET && estado_anterior_botao1 == GPIO_PIN_SET){
        if (matricula == 0) {
            ST7735_WriteString(5, 100, "ERRO: Sem Digital ", Font_7x10, RED, WHITE);
            HAL_Delay(1000);
            atualizar_tela_principal();
        }
        else if (alunos_presentes >= limite_maximo_alunos){
            ST7735_WriteString(5, 100, "ERRO: Sala Cheia  ", Font_7x10, RED, WHITE);
            HAL_Delay(1000);
            atualizar_tela_principal();
        }
        else {
            alunos_presentes++;
            total_entrada++;
            matricula = 0;
            atualizar_tela_principal();
        }
        HAL_Delay(50);
    }
    estado_anterior_botao1 = atual_b1;


    GPIO_PinState atual_b2 = HAL_GPIO_ReadPin(BOTAO2_GPIO_Port, BOTAO2_Pin);
    if (atual_b2 == GPIO_PIN_RESET && estado_anterior_botao2 == GPIO_PIN_SET){
        if (alunos_fora >= 3) {
            ST7735_WriteString(5, 100, "ERRO: Limite Banho", Font_7x10, RED, WHITE);
            HAL_Delay(1000);
            atualizar_tela_principal();
        }
        else if (alunos_presentes > 0) {
            alunos_presentes--;
            alunos_fora++;
            total_saida++;
            atualizar_tela_principal();
        }
        HAL_Delay(50);
    }
    estado_anterior_botao2 = atual_b2;

    GPIO_PinState atual_b3 = HAL_GPIO_ReadPin(BOTAO3_GPIO_Port, BOTAO3_Pin);
    if (atual_b3 == GPIO_PIN_RESET && estado_anterior_botao3 == GPIO_PIN_SET){
        if (alunos_fora > 0){
            alunos_fora--;
            alunos_presentes++;
            atualizar_tela_principal();
        }
        HAL_Delay(50);
    }
    estado_anterior_botao3 = atual_b3;
}

void exibir_relatorio_final(void){
    char buffer[32];

    ST7735_FillScreen(BLACK);
    ST7735_WriteString(5, 10, "AULA ENCERRADA", Font_7x10, WHITE, BLACK);
    ST7735_DrawLine(0, 25, 128, 25, WHITE);

    sprintf(buffer, "Total Alunos: %d", total_entrada);
    ST7735_WriteString(5, 45, buffer, Font_7x10, WHITE, BLACK);

    sprintf(buffer, "Qtd Saidas: %d", total_saida);
    ST7735_WriteString(5, 65, buffer, Font_7x10, WHITE, BLACK);

    sprintf(buffer, "Nao voltaram: %d", alunos_fora);
    ST7735_WriteString(5, 85, buffer, Font_7x10, WHITE, BLACK);

    ST7735_WriteString(5, 115, "Reinicie o sistema", Font_7x10, YELLOW, BLACK);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
