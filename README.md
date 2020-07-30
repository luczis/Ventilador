# IHM-Respirador

Projeto para uma interface Homem-Máquina desenvolvida a fim de ser utilizada para um respirador artíficial, voltado para um ESP32. Desenvolvido em nome da UTFPR-Câmpus Apucarana.

# Como utilizar

Faça download dos arquivos, e certifique-se de que tudo esta em uma pasta de mesmo nome do arquivo .ino (O Arduino IDE reclama caso contrário).

Compile o .ino para o ESP32 (Você precisa de arquivos adicionais para compilar pra esta placa, não está inclusa no pacote base do Arduino IDE).

*Arquivo .json indicado para o ESP32: https://dl.espressif.com/dl/package_esp32_index.json*

Pronto! Na teoria o microcontrolador deve mostrar imagens no seu monitor VGA.

# Bugs conhecidos

# TODO

-Incrementar funções para leitura de botões e encoder;

-Configurar GPIOs para botões, e algumas saída;

-Incrementar leitura de 3 A/Ds (1.1-3.9V) (Evitar pinos que possuam interrupções);

-Habilitar interrupções;

-Opção para aspecto 16:9 (Atualmente somente para 4:3).

# Colaboradores

Bruno Gabriel da Silva

Lucas Zischler
