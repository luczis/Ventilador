# IHM-Respirador

Projeto para uma interface Homem-Máquina desenvolvida a fim de ser utilizada para um respirador artíficial, voltado para um ESP32. Desenvolvido em nome da UTFPR-Câmpus Apucarana.

# Como utilizar

Faça download dos arquivos, e certifique-se de que tudo esta em uma pasta de mesmo nome do arquivo .ino (O Arduino IDE reclama caso contrário).

Compile o .ino para o ESP32 (Você precisa de arquivos adicionais para compilar pra esta placa, não está inclusa no pacote base do Arduino IDE).

*Arquivo .json indicado para o ESP32: https://dl.espressif.com/dl/package_esp32_index.json*

A imagem abaixo mostra quais pinos do ESP32 devem ser conectados no conector VGA.

![14 R, 19 G, 27 B, 26 VSYNC, 25 HSYNC](./VGA.svg)

Pronto! Na teoria o microcontrolador deve mostrar imagens no seu monitor VGA.

# Bugs conhecidos

-Nenhum ... No momento ...

# TODO

-Tela inicial de setup de parâmetros;

-Definir os parâmetros nas caixas, botões e gráficos;

<del>Incrementar funções para leitura de botões e encoder;</del>

<del>Configurar GPIOs para botões, e algumas saída;</del>

<del>Incrementar leitura de 3 A/Ds (0-3.0V)</del>

<del>Habilitar interrupções;</del>

-Opção para aspecto 16:9 (Atualmente somente para 4:3).

# Colaboradores

Bruno Gabriel da Silva

Lucas Zischler
