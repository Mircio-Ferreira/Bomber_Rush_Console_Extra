# Arduino RFID Controller para Bomber Rush

Este projeto contém um código para **Arduino Leonardo** que usa um **sensor RFID** para ler cartões e ativar eventos dentro do jogo **Bomber Rush**.

O Arduino funciona como um teclado USB: quando um cartão RFID é lido, ele envia uma tecla para o computador, e o jogo interpreta isso como uma ação especial.

O Bomber Rush é um jogo em C disponível aqui:  
https://github.com/iampassos/bomber-rush

A ideia é permitir que cartões RFID atuem como “power-ups” ou gatilhos de eventos dentro do jogo, criando uma interação física extra.
