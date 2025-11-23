#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- VARIABLES GLOBALES (Registros MIPS) ---
int tamanoTablero;
int tablero[120];

// Jugador (J)
int posJ = 0;
int dineroTotalJ = 0;
int dineroGanadoTurnoJ = 0; // Nuevo: para cumplir Regla 6
int tesorosJ = 0;
int finJ = 0;

// Maquina (M)
int posM = 0;
int dineroTotalM = 0;
int dineroGanadoTurnoM = 0; // Nuevo: para cumplir Regla 6
int tesorosM = 0;
int finM = 0;

// Función para imprimir el reporte completo (Regla 6)
// En MIPS esto será una subrutina llena de syscalls 4 (string) y 1 (int)
void mostrarEstado() {
    printf("\n================ ESTADO DEL JUEGO ================\n");
    
    // Estado Jugador
    printf("JUGADOR -> Pos: %d | Gano este turno: $%d | Total: $%d | Tesoros: %d\n", 
           posJ, dineroGanadoTurnoJ, dineroTotalJ, tesorosJ);
           
    // Estado Maquina
    printf("MAQUINA -> Pos: %d | Gano este turno: $%d | Total: $%d | Tesoros: %d\n", 
           posM, dineroGanadoTurnoM, dineroTotalM, tesorosM);
           
    printf("==================================================\n");
}

void configurarTablero() {
    // 1. Llenar todo con dinero aleatorio ($10-$100)
    for(int i=0; i < tamanoTablero; i++) {
        tablero[i] = (rand() % 91) + 10; 
    }

    // 2. Colocar Tesoros (30%)
    int numTesoros = (tamanoTablero * 30) / 100;
    int puestos = 0;
    while(puestos < numTesoros) {
        int idx = rand() % tamanoTablero;
        if(tablero[idx] != -1) { 
            tablero[idx] = -1;   
            puestos++;
        }
    }
}

void realizarMovimiento(int esJugador) {
    int avance;
    int *pos, *dineroTotal, *dineroTurno, *tesoros, *fin;

    // Mapeo de punteros para no repetir codigo (En MIPS usarás registros base)
    if(esJugador) {
        printf("\n--- TU TURNO ---\nCuanto avanzas (1-6)? ");
        scanf("%d", &avance);
        // Validacion simple (en MIPS hacer un bucle)
        while(avance < 1 || avance > 6) {
            printf("Invalido. (1-6): ");
            scanf("%d", &avance);
        }
        pos = &posJ; dineroTotal = &dineroTotalJ; dineroTurno = &dineroGanadoTurnoJ; 
        tesoros = &tesorosJ; fin = &finJ;
    } else {
        printf("\n--- TURNO MAQUINA ---\n");
        avance = (rand() % 6) + 1;
        printf("Maquina lanza dado: %d\n", avance);
        pos = &posM; dineroTotal = &dineroTotalM; dineroTurno = &dineroGanadoTurnoM; 
        tesoros = &tesorosM; fin = &finM;
    }

    // Reiniciar ganancia del turno actual
    *dineroTurno = 0;

    if(*fin) return; // Si ya terminó, no hace nada

    *pos += avance;

    // Validar limite tablero
    if(*pos >= tamanoTablero - 1) {
        *pos = tamanoTablero - 1;
        *fin = 1;
        printf(">>> %s LLEGO AL FINAL! <<<\n", esJugador ? "JUGADOR" : "MAQUINA");
    } else {
        // Logica de items
        int valorCasilla = tablero[*pos];
        
        if(valorCasilla == -1) {
            (*tesoros)++;
            tablero[*pos] = 0; // Consumir item
            // No gana dinero, gana tesoro
        } else if(valorCasilla > 0) {
            *dineroTurno = valorCasilla; // Guardamos lo ganado en este turno
            *dineroTotal += valorCasilla;
            tablero[*pos] = 0; // Consumir dinero
        }
    }
    
    // AQUI SE CUMPLE LA REGLA 6: Mostrar estado al finalizar el movimiento
    mostrarEstado();
}

int main() {
    srand(time(0));

    // 1. Configuración inicial
    do {
        printf("Ingrese tamano del tablero (20-120): ");
        scanf("%d", &tamanoTablero);
    } while(tamanoTablero < 20 || tamanoTablero > 120);

    configurarTablero();
    
    printf("\nINICIO DEL JUEGO\n");
    mostrarEstado(); // Mostrar estado inicial (todo en 0)

    // 2. Bucle Principal
    while(1) {
        // --- Turno Jugador ---
        if(!finJ) realizarMovimiento(1);
        
        // Verificar victoria inmediata por tesoros
        if(tesorosJ == 3) break;
        if(finJ && finM) break;

        // --- Turno Maquina ---
        if(!finM) realizarMovimiento(0);

        // Verificar victoria inmediata por tesoros
        if(tesorosM == 3) break;
        if(finJ && finM) break;
    }

    // 3. Determinar Ganador (Reglas 9 y 10)
    printf("\n************ JUEGO TERMINADO ************\n");
    int ganoJugador = 0;

    if(tesorosJ == 3) {
        printf("Ganaste por encontrar 3 TESOROS!\n");
        ganoJugador = 1;
    } else if(tesorosM == 3) {
        printf("La maquina gana por encontrar 3 TESOROS!\n");
        ganoJugador = 0;
    } else {
        // Empate o fin de tablero -> Gana quien tiene mas dinero
        printf("Nadie obtuvo 3 tesoros. Se decide por DINERO.\n");
        if(dineroTotalJ > dineroTotalM) ganoJugador = 1;
        else if(dineroTotalM > dineroTotalJ) ganoJugador = 0;
        else {
            // Empate dinero -> tesoros
            if(tesorosJ >= tesorosM) ganoJugador = 1;
        }
    }

    int pozo = dineroTotalJ + dineroTotalM;
    if(ganoJugador) printf("RESULTADO: GANASTE $%d (Tu dinero + Maquina)\n", pozo);
    else printf("RESULTADO: PERDISTE. La maquina se lleva $%d\n", pozo);

    return 0;
}
