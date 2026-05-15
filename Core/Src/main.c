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
int semente_definida = 0;
int sistema_autenticado = 0;
int config_finalizada = 0;
int senha_gerada = 0;
int tentativas_erradas = 0;
int bloqueado = 0;
int tempo_bloqueio = 0;

int limite_alunos = 0;
int total_entrada = 0;
int total_saida = 0;
int alunos_fora = 0;
int matricula = 0;

int posicao_digito = 0;
int digito_atual = 0;
int senha_inserida = 0;
char buffer[32];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
void inserir_senha(void);
void gerenciar_configuracao(void);
void gerenciar_aula(void);
void exibir_relatorio_final(void);
void atualizar_tela_inserir(int pos, int dig);
void atualizar_dados_tela_aula(char* msg, int cor_msg);
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
  ST7735_FillScreen(BLACK);
  /* USER CODE END 2 */
  ST7735_Init();
  ST7735_FillScreen(BLACK);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
      {
	        if (aula_iniciada == 0) {
	          ST7735_WriteString(5, 20, "SISTEMA PRONTO", Font_7x10, CYAN, BLACK);
	          ST7735_WriteString(5, 50, "Pressione B1 para", Font_7x10, WHITE, BLACK);
	          ST7735_WriteString(5, 70, "INICIAR A AULA", Font_7x10, YELLOW, BLACK);

	          if (HAL_GPIO_ReadPin(BOTAO1_GPIO_Port, BOTAO1_Pin) == GPIO_PIN_RESET) {
	            srand(HAL_GetTick());
	            semente_definida = 1;
	            senha_gerada = (rand() % 9000) + 1000;
	            aula_iniciada = 1;

	            senha_inserida = 0;
	            posicao_digito = 0;
	            digito_atual = 0;

	            while(HAL_GPIO_ReadPin(BOTAO1_GPIO_Port, BOTAO1_Pin) == GPIO_PIN_RESET);
	            ST7735_FillScreen(BLACK);
	            atualizar_tela_inserir(posicao_digito, digito_atual);
	          }
	        }

	        else if (sistema_autenticado == 0) {
	          inserir_senha();
	        }

	        else if (config_finalizada == 0) {
	          gerenciar_configuracao();
	        }

	        else {
	          if (HAL_GPIO_ReadPin(BOTAO1_GPIO_Port, BOTAO1_Pin) == GPIO_PIN_RESET &&
	              HAL_GPIO_ReadPin(BOTAO4_GPIO_Port, BOTAO4_Pin) == GPIO_PIN_RESET) {
	            exibir_relatorio_final();
	            while(1);
	          }
	          gerenciar_aula();
	        }

	        /* USER CODE END WHILE */
      }
  	  /* USER CODE BEGIN 3 */
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
void atualizar_tela_inserir(int pos, int dig) {
    ST7735_WriteString(5, 10, "--- AUTENTICACAO ---", Font_7x10, MAGENTA, BLACK);

    sprintf(buffer, "SENHA: %d    ", senha_gerada);
    ST7735_WriteString(5, 35, buffer, Font_7x10, GREEN, BLACK);

    sprintf(buffer, "DIGITO: %d   ", dig);
    ST7735_WriteString(5, 65, buffer, Font_7x10, YELLOW, BLACK);

    ST7735_WriteString(5, 95, "INPUT: ", Font_7x10, WHITE, BLACK);

    for(int i = 0; i < 4; i++) {
        if(i < pos) {
            ST7735_WriteString(65 + (i * 12), 95, "*", Font_7x10, CYAN, BLACK);
        } else {
            ST7735_WriteString(65 + (i * 12), 95, "_", Font_7x10, WHITE, BLACK);
        }
    }

    ST7735_WriteString(5, 120, "B1:+  B2:CONFIRMA", Font_7x10, WHITE, BLACK);
}

void exibir_tela_inicial(void) {
    ST7735_FillScreen(BLACK);
    ST7735_WriteString(10, 40, "BEM-VINDO!", Font_11x18, WHITE, BLACK);
    ST7735_WriteString(10, 70, "Pressione B1", Font_7x10, YELLOW, BLACK);
    ST7735_WriteString(10, 85, "para iniciar", Font_7x10, YELLOW, BLACK);
}

void gerar_senha_aleatoria(void){
	senha_gerada = (rand() % 9000) + 1000;
}

void inserir_senha(void) {

    if (bloqueado) {
        if (HAL_GetTick() - tempo_bloqueio >= 10000) {
            bloqueado = 0;
            tentativas_erradas = 0;
            ST7735_FillScreen(BLACK);
            atualizar_tela_inserir(posicao_digito, digito_atual);
        } else {
            sprintf(buffer, "BLOQUEADO: %ds ", (int)(10 - (HAL_GetTick() - tempo_bloqueio) / 1000));
            ST7735_WriteString(5, 65, buffer, Font_7x10, RED, BLACK);
            return;
        }
    }

    if (HAL_GPIO_ReadPin(BOTAO1_GPIO_Port, BOTAO1_Pin) == GPIO_PIN_RESET) {
        HAL_Delay(50);
        digito_atual = (digito_atual + 1) % 10;
        atualizar_tela_inserir(posicao_digito, digito_atual);
        while(HAL_GPIO_ReadPin(BOTAO1_GPIO_Port, BOTAO1_Pin) == GPIO_PIN_RESET);
    }

    if (HAL_GPIO_ReadPin(BOTAO2_GPIO_Port, BOTAO2_Pin) == GPIO_PIN_RESET) {
        HAL_Delay(50);
        senha_inserida = (senha_inserida * 10) + digito_atual;
        posicao_digito++;
        digito_atual = 0;

        if (posicao_digito == 4) {
            if (senha_inserida == senha_gerada) {
                sistema_autenticado = 1;
                ST7735_FillScreen(BLACK);
                ST7735_WriteString(5, 50, "ACESSO LIBERADO", Font_7x10, GREEN, BLACK);
                HAL_Delay(1000);
                ST7735_FillScreen(BLACK);
            } else {
                tentativas_erradas++;
                senha_inserida = 0;
                posicao_digito = 0;
                if (tentativas_erradas >= 3) {
                    bloqueado = 1;
                    tempo_bloqueio = HAL_GetTick();
                    ST7735_FillScreen(BLACK);
                } else {
                    ST7735_FillScreen(BLACK);
                    ST7735_WriteString(5, 50, "SENHA INCORRETA", Font_7x10, RED, BLACK);
                    HAL_Delay(1000);
                    ST7735_FillScreen(BLACK);
                    atualizar_tela_inserir(posicao_digito, digito_atual);
                }
            }
        } else {
            atualizar_tela_inserir(posicao_digito, digito_atual);
        }
        while(HAL_GPIO_ReadPin(BOTAO2_GPIO_Port, BOTAO2_Pin) == GPIO_PIN_RESET);
    }
}

void gerenciar_configuracao(void) {
    ST7735_WriteString(5, 10, "CONFIG. LIMITE", Font_7x10, WHITE, BLACK);
    ST7735_WriteString(5, 90, "B2: + | B3: CONFIRMA", Font_7x10, YELLOW, BLACK);

    if (HAL_GPIO_ReadPin(BOTAO2_GPIO_Port, BOTAO2_Pin) == GPIO_PIN_RESET) {
        limite_alunos++;
        sprintf(buffer, "Limite: %d  ", limite_alunos);
        ST7735_WriteString(5, 50, buffer, Font_7x10, CYAN, BLACK);
        HAL_Delay(200);
        while(HAL_GPIO_ReadPin(BOTAO2_GPIO_Port, BOTAO2_Pin) == GPIO_PIN_RESET);
    }
    if (HAL_GPIO_ReadPin(BOTAO3_GPIO_Port, BOTAO3_Pin) == GPIO_PIN_RESET && limite_alunos > 0) {
        config_finalizada = 1;
        ST7735_FillScreen(BLACK);
        atualizar_dados_tela_aula("AULA INICIADA", GREEN);
        while(HAL_GPIO_ReadPin(BOTAO3_GPIO_Port, BOTAO3_Pin) == GPIO_PIN_RESET);
    }
}

void gerenciar_aula(void) {
    int mudou = 0;
    char* msg = "EM CURSO...";
    int cor = WHITE;

    if (HAL_GPIO_ReadPin(BOTAO4_GPIO_Port, BOTAO4_Pin) == GPIO_PIN_RESET) {
        matricula = 1;
        msg = "DIGITAL OK";
        cor = CYAN;
        mudou = 1;
        while(HAL_GPIO_ReadPin(BOTAO4_GPIO_Port, BOTAO4_Pin) == GPIO_PIN_RESET);
    }
    if (HAL_GPIO_ReadPin(BOTAO1_GPIO_Port, BOTAO1_Pin) == GPIO_PIN_RESET) {
        if (matricula && total_entrada < limite_alunos) {
            total_entrada++;
            matricula = 0;
            msg = "BEM-VINDO";
            cor = GREEN;
        } else {
            msg = matricula ? "LOTADO!" : "SEM DIGITAL";
            cor = RED;
        }
        mudou = 1;
        while(HAL_GPIO_ReadPin(BOTAO1_GPIO_Port, BOTAO1_Pin) == GPIO_PIN_RESET);
    }
    if (HAL_GPIO_ReadPin(BOTAO2_GPIO_Port, BOTAO2_Pin) == GPIO_PIN_RESET) {
        if (alunos_fora < 3) {
            alunos_fora++;
            total_saida++;
            msg = "SAIDA BANHEIRO";
            cor = YELLOW;
        } else {
            msg = "WC BANHEIRO";
            cor = RED;
        }
        mudou = 1;
        while(HAL_GPIO_ReadPin(BOTAO2_GPIO_Port, BOTAO2_Pin) == GPIO_PIN_RESET);
    }
    if (HAL_GPIO_ReadPin(BOTAO3_GPIO_Port, BOTAO3_Pin) == GPIO_PIN_RESET) {
        if (alunos_fora > 0) {
            alunos_fora--;
            msg = "RETORNO WC";
            cor = GREEN;
        }
        mudou = 1;
        while(HAL_GPIO_ReadPin(BOTAO3_GPIO_Port, BOTAO3_Pin) == GPIO_PIN_RESET);
    }

    if (mudou) atualizar_dados_tela_aula(msg, cor);
}

void atualizar_dados_tela_aula(char* msg, int cor_msg) {
    ST7735_WriteString(5, 10, "CONTROLE DE AULA", Font_7x10, MAGENTA, BLACK);
    ST7735_WriteString(5, 40, "            ", Font_7x10, BLACK, BLACK);
    ST7735_WriteString(5, 40, msg, Font_7x10, cor_msg, BLACK);
    sprintf(buffer, "Alunos: %d/%d", total_entrada, limite_alunos);
    ST7735_WriteString(5, 70, buffer, Font_7x10, WHITE, BLACK);
    sprintf(buffer, "No banheiro: %d/3", alunos_fora);
    ST7735_WriteString(5, 95, buffer, Font_7x10, (alunos_fora == 3 ? RED : GREEN), BLACK);
}

void exibir_relatorio_final(void) {
    ST7735_FillScreen(BLACK);
    ST7735_WriteString(5, 10, "RELATORIO FINAL", Font_7x10, CYAN, BLACK);
    sprintf(buffer, "Presencas: %d", total_entrada);
    ST7735_WriteString(5, 40, buffer, Font_7x10, WHITE, BLACK);
    sprintf(buffer, "Saidas WC: %d", total_saida);
    ST7735_WriteString(5, 65, buffer, Font_7x10, WHITE, BLACK);
    sprintf(buffer, "Nao voltaram: %d", alunos_fora);
    ST7735_WriteString(5, 90, buffer, Font_7x10, RED, BLACK);
    ST7735_WriteString(5, 115, "AULA ENCERRADA", Font_7x10, YELLOW, BLACK);
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
