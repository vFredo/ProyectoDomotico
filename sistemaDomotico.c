#include "phtrdsMsgLyr.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_CASAS 3
#define NUM_SENSOR_HUMEDAD 3

/***( Function prototypes )***********************************************/
static void *pSensorMovimiento(void *arg);
static void *pSensorHumo(void *arg);
static void *pSensorHumedad(void *arg);
static void *pSensorFotoelectrico(void *arg);
static void *pSensorRFID(void *arg);
static void *pRegado(void *arg);
static void *pPanelAcceso(void *arg);
static void *pControladorCasa(void *arg);
static void *pSistema(void *arg);
static void *pEnvironment(void *arg);

/***( Auxiliar processes )************************************************/
static void *abrirPuerta(void *arg);

/***( Main process )******************************************************/
int main(void) {
  pthread_t foto_tid, rfid_tid;
  pthread_t sistema_tid;
  pthread_t panel_acc_tid;
  pthread_t env_tid;

  /* Create queues */
  initialiseQueues();

  system("clear");

  /* Create threads */
  pthread_create(&sistema_tid, NULL, pSistema, NULL);
  pthread_create(&foto_tid, NULL, pSensorFotoelectrico, NULL);
  pthread_create(&rfid_tid, NULL, pSensorRFID, NULL);
  pthread_create(&panel_acc_tid, NULL, pPanelAcceso, NULL);
  sleep(5); // Dar tiempo a inicializar todos los procesos
  pthread_create(&env_tid, NULL, pEnvironment, NULL);

  /* Wait for threads to finish */
  pthread_join(panel_acc_tid, NULL);
  pthread_join(foto_tid, NULL);
  pthread_join(rfid_tid, NULL);
  pthread_join(sistema_tid, NULL);
  pthread_join(env_tid, NULL);

  /* Destroy queues */
  destroyQueues();
  return 0;
}

/***( SDL system processes )**********************************************/
static void *pEnvironment(void *arg) {
  char line[100];
  int choice, exit = 1;
  msg_t OutMsg;

  while (exit) {
    sleep(1);
    puts("\nEnvironment:                     Sistema Residencial Domotico\n"
         "-------------------------------------------------------------\n"
         "1. Activar sensor movimiento de casa\n"
         "2. Nivel de humo (Sensor humo)\n"
         "3. Riego plantas (Sensor humedad)\n"
         "4. Ingreso carro (Sensor foto electricto)\n"
         "5. Ingreso residente (Sensor RFID)\n"
         "6. Ingreso visitante (Panel Acceso)\n"
         "7. Seguridad casa On/Off\n"
         "8. Llega policia a la unidad residencial\n"
         "9. Residente acepta/rechaza invitado\n"
         "10. Exit");
    fflush(stdout);

    fflush(stdin);
    fgets(line, sizeof(line), stdin);
    sscanf(line, "%d", &choice);

    switch (choice) {
    case 1:
      printf("Elija que casa enviar la señal selecciones entre 1-%d\n",
             NUM_CASAS);
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%lf", &OutMsg.value);

      OutMsg.signal = sMovimiento;
      OutMsg.value += NUM_CASAS;
      sendMessage(&(queue[(int)OutMsg.value]), OutMsg);

      break;
    case 2:
      printf("Elija la casa la cual se le envia la señal entre 1-%d\n",
             NUM_CASAS);
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%d", &OutMsg.sender);

      OutMsg.sender += NUM_CASAS * 2;

      printf("Digite un valor para el nivel de humo entre 0.0 y 1.0:\n");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%lf", &OutMsg.value);

      OutMsg.signal = sNivelHumo;
      sendMessage(&(queue[OutMsg.sender]), OutMsg);
      break;
    case 3:
      printf("Digite la zona del sensor que quiere activar [1-3]\n");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%d", &OutMsg.sender);
      OutMsg.sender += 9;

      printf("Digite un valor para la humedad de 0.0 a 1.0:\n");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%lf", &OutMsg.value);

      OutMsg.signal = sNivelHumedad;
      sendMessage(&(queue[OutMsg.sender]), OutMsg);
      break;
    case 4:
      OutMsg.signal = sLlegaCarro;
      sendMessage(&(queue[SENSOR_FOTOELECTRICO]), OutMsg);

      printf("Digite la placa del carro:\n");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%s", OutMsg.placa);

      sleep(1);
      OutMsg.signal = sLecturaCamara;
      sendMessage(&(queue[SISTEMA]), OutMsg);
      break;
    case 5:
      printf("Digite el codigo de la tarjeta RFID:\n");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%lf", &OutMsg.value);

      OutMsg.signal = sTarjetaEnSensor;
      sendMessage(&(queue[SENSOR_RFID]), OutMsg);
      break;
    case 6:
      printf("Elija la casa la cual se le envia la señal entre 1-%d\n",
             NUM_CASAS);
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%d", &OutMsg.sender);

      OutMsg.signal = sVisistanteSoliticaAcceso;
      sendMessage(&(queue[PANEL_ACCESO]), OutMsg);
      break;
    case 7:
      printf("Elija la casa la cual se le envia la señal entre 1-%d\n",
             NUM_CASAS);
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%d", &OutMsg.sender);

      puts("0. Desactivar seguridad\n"
           "1. Activar seguridad");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%lf", &OutMsg.value);

      OutMsg.signal = sSeguridadActiva;
      sendMessage(&(queue[OutMsg.sender]), OutMsg);
      break;
    case 8:

      printf("Elija la casa la cual llega la policia entre 1-%d\n", NUM_CASAS);
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%d", &OutMsg.sender);

      OutMsg.signal = sLlegaPolicia;
      sendMessage(&(queue[SISTEMA]), OutMsg);
      break;
    case 9:
      printf("Elija la casa la cual se le envia la señal entre 1-%d\n",
             NUM_CASAS);
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%d", &OutMsg.sender);

      puts("\n0. Rechazar Invitado.\n"
           "1. Aceptar Invitado.");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%lf", &OutMsg.value);

      OutMsg.signal = sRespuestaResidente;
      sendMessage(&(queue[OutMsg.sender]), OutMsg);
      break;
    case 10:
      OutMsg.signal = sExitPanelAcceso;
      sendMessage(&(queue[PANEL_ACCESO]), OutMsg);

      OutMsg.signal = sExitFotoElectrico;
      sendMessage(&(queue[SENSOR_FOTOELECTRICO]), OutMsg);

      OutMsg.signal = sExitRFID;
      sendMessage(&(queue[SENSOR_RFID]), OutMsg);

      OutMsg.signal = sExitSistema;
      sendMessage(&(queue[SISTEMA]), OutMsg);

      printf("Saliendo del programa...\n");
      fflush(stdout);

      exit = 0;
      break;
    default:
      printf("Opcion no valida. Intente de nuevo...\n");
      fflush(stdout);
      break;
    }
  }
  return NULL;
}

static void *pSensorMovimiento(void *arg) {
  SENSOR_MOVIMIENTO_ESTADOS state, state_next;
  msg_t InMsg, OutMsg;
  state_next = IdleMov;
  parametros par = *(parametros *)arg;
  int stop = 1;

  while (stop) {
    state = state_next;
    InMsg = receiveMessage(&(queue[par.idSensor]));
    switch (state) {
    case IdleMov:
      switch (InMsg.signal) {
      case sMovimiento:
        printf("\t\t\t Sensor Movimiento %d -> Casa %d.\n", par.idSensor,
               par.idParent);
        fflush(stdout);
        OutMsg.signal = sMovimientoCasa;
        sendMessage(&(queue[par.idParent]), OutMsg);
        state_next = IdleMov;
        break;
      case sExitMov:
        stop = 0;
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  return NULL;
}

static void *pSensorHumo(void *arg) {
  SENSOR_HUMO_ESTADOS state, state_next;
  msg_t InMsg, OutMsg;
  state_next = IdleHumo;
  parametros par = *(parametros *)arg;
  int stop = 1;

  while (stop) {
    state = state_next;
    InMsg = receiveMessage(&(queue[par.idSensor]));
    switch (state) {
    case IdleHumo:
      switch (InMsg.signal) {
      case sNivelHumo:
        printf("\t\t\t Sensor Humo %d -> Casa %d.\n", par.idSensor,
               par.idParent);
        fflush(stdout);
        OutMsg.signal = sNivelHumoCasa;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[par.idParent]), OutMsg);
        state_next = IdleHumo;
        break;
      case sExitHumo:
        stop = 0;
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  return NULL;
}

static void *pSensorHumedad(void *arg) {
  SENSOR_HUMEDAD_ESTADOS state, state_next;
  state_next = IdleHumedad;
  int stop = 1;

  msg_t InMsg, OutMsg;
  int zona = *(int *)arg;
  while (stop) {
    state = state_next;
    InMsg = receiveMessage(&(queue[10 + zona - 1]));
    switch (state) {
    case IdleHumedad:
      switch (InMsg.signal) {
      case sNivelHumedad:
        printf("\t\t\t Sensor Humedad %d -> Sistema.\n", zona);
        fflush(stdout);

        OutMsg.sender = zona;
        OutMsg.signal = sNivelHumedadSistema;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[SISTEMA]), OutMsg);
        state_next = IdleHumedad;
        break;
      case sExitHumedad:
        stop = 0;
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  return NULL;
}

static void *pSensorFotoelectrico(void *arg) {
  SENSOR_FOTOELECTRICO_ESTADOS state, state_next;
  msg_t InMsg, OutMsg;
  state_next = IdleFoto;
  int stop = 1;

  while (stop) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SENSOR_FOTOELECTRICO]));
    switch (state) {
    case IdleFoto:
      switch (InMsg.signal) {
      case sLlegaCarro:
        printf("\t\t\t Sensor Fotoelectrico -> Sistema.\n");
        fflush(stdout);

        OutMsg.signal = sLlegaCarroSistema;
        sendMessage(&(queue[SISTEMA]), OutMsg);
        state_next = IdleFoto;
        break;
      case sExitFotoElectrico:
        stop = 0;
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  return NULL;
}

static void *pSensorRFID(void *arg) {
  SENSOR_RFID_ESTADOS state, state_next;
  msg_t InMsg, OutMsg;
  state_next = IdleRFID;
  int stop = 1;

  while (stop) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SENSOR_RFID]));
    switch (state) {
    case IdleRFID:
      switch (InMsg.signal) {
      case sTarjetaEnSensor:
        printf("\t\t\t Sensor RFID -> Sistema.\n");
        fflush(stdout);

        OutMsg.signal = sLecturaRFID;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[SISTEMA]), OutMsg);
        state_next = IdleRFID;
        break;
      case sExitRFID:
        stop = 0;
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  return NULL;
}

static void *pPanelAcceso(void *arg) {
  PANEL_ACCESO_ESTADOS state, state_next;
  msg_t InMsg, OutMsg;
  state_next = IdlePanel;
  int exit = 1;

  while (exit) {
    state = state_next;
    InMsg = receiveMessage(&(queue[PANEL_ACCESO]));
    switch (state) {
    case IdlePanel:
      switch (InMsg.signal) {
      case sVisistanteSoliticaAcceso:
        printf("\t\t\t (Panel de Acceso Casa %d -> Sistema)\n", InMsg.sender);
        fflush(stdout);

        OutMsg.signal = sSolicitud;
        OutMsg.sender = InMsg.sender;
        sendMessage(&(queue[SISTEMA]), OutMsg);
        state_next = IdlePanel;
        break;
      case sExitPanelAcceso:
        exit = 0;
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  return NULL;
}

static void *pControladorCasa(void *arg) {
  CASA_ESTADOS state, state_next;
  msg_t InMsg, OutMsg;
  int seguridad_activa = 0;
  int alarmaRobo = 0, alarmaHumo = 0;
  int idCasa = *(int *)arg;
  pthread_t mov_pid, humo_pid;

  parametros pr_mov;
  pr_mov.idParent = idCasa;
  pr_mov.idSensor = idCasa + NUM_CASAS;
  pthread_create(&mov_pid, NULL, pSensorMovimiento, (void *)&pr_mov);

  parametros pr_humo;
  pr_humo.idParent = idCasa;
  pr_humo.idSensor = idCasa + NUM_CASAS * 2;
  pthread_create(&humo_pid, NULL, pSensorHumo, (void *)&pr_humo);

  state_next = IdleCasa;
  int exit = 1;
  while (exit) {
    state = state_next;
    InMsg = receiveMessage(&(queue[idCasa]));

    switch (state) {
    case IdleCasa:
      switch (InMsg.signal) {
      case sExitCasa:
        OutMsg.signal = sExitMov;
        sendMessage(&(queue[idCasa + NUM_CASAS]), OutMsg);

        OutMsg.signal = sExitHumo;
        sendMessage(&(queue[idCasa + NUM_CASAS * 2]), OutMsg);

        exit = 0;
        break;
      case sNotificarCasa:
        printf("\t\t\t Notificando al residente de una visita.\n"
               "\t\t\t (Casa %d -> Residente)\n",
               idCasa);
        fflush(stdout);
        state_next = EsperandoRespuestaResidente;
        break;
      case sRoboDetenido:
        if (alarmaRobo == 1) {
          printf("\t\t\t Alarma de Robo desactivada en Casa %d.\n", idCasa);
          fflush(stdout);
          alarmaRobo = 0;
        } else {
          printf("\t\t\t La Casa %d no hay robos.\n", idCasa);
          fflush(stdout);
        }
        state_next = IdleCasa;
        break;
      case sNivelHumoCasa:
        if (InMsg.value >= 0.7) {
          printf("\t\t\t Alarma de humo activada. (Casa %d -> Sistema)\n",
                 idCasa);
          fflush(stdout);

          alarmaHumo = 1;
          OutMsg.signal = sIncendio;
          OutMsg.value = idCasa;
          sendMessage(&(queue[SISTEMA]), OutMsg);
        } else {
          printf("\t\t\t Alarma de humo desactivada casa %d.\n", idCasa);
          fflush(stdout);
          alarmaHumo = 0;
        }
        state_next = IdleCasa;
        break;
      case sMovimientoCasa:
        if (seguridad_activa == 1) {
          printf("\t\t\t Alarma de Robo activada (Casa %d -> Sistema)\n",
                 idCasa);
          fflush(stdout);

          alarmaRobo = 1;
          OutMsg.signal = sRobo;
          OutMsg.value = idCasa;
          sendMessage(&(queue[SISTEMA]), OutMsg);
        } else {
          printf("\t\t\t La seguridad esta desactivada en Casa %d.\n", idCasa);
          fflush(stdout);
        }
        state_next = IdleCasa;
        break;
      case sSeguridadActiva:
        switch ((int)InMsg.value) {
        case 0:
          if (alarmaRobo == 1) {
            printf("\t\t\t No se puede desactivar seguridad en Casa %d, Alarma "
                   "de robo "
                   "activa.\n",
                   idCasa);
            fflush(stdout);
          } else {
            printf("\t\t\t Seguridad desactivada Casa %d.\n", idCasa);
            fflush(stdout);
            seguridad_activa = 0;
          }
          break;
        case 1:
          if (seguridad_activa != 1) {
            printf("\t\t\t Seguridad activada en Casa %d.\n", idCasa);
            fflush(stdout);
            seguridad_activa = 1;
          } else {
            printf("\t\t\t La seguridad ya esta activada en Casa %d.\n",
                   idCasa);
            fflush(stdout);
          }
          break;
        default:
          printf("\t\t\t Opcion invalida casa %d.\n", idCasa);
          fflush(stdout);
          break;
        }
        state_next = IdleCasa;
        break;
      default:
        break;
      }
      break;
    case EsperandoRespuestaResidente:
      switch (InMsg.signal) {
      case sRespuestaResidente:
        printf("\t\t\t Respuesta Residente (Casa %d -> Sistema).\n", idCasa);
        fflush(stdout);

        OutMsg.signal = sInvitadoAcepta;
        OutMsg.value = InMsg.value;
        OutMsg.sender = idCasa;
        sendMessage(&(queue[SISTEMA]), OutMsg);
        state_next = IdleCasa;
        break;
      case sExitCasa:
        OutMsg.signal = sExitMov;
        sendMessage(&(queue[idCasa + NUM_CASAS]), OutMsg);

        OutMsg.signal = sExitHumo;
        sendMessage(&(queue[idCasa + NUM_CASAS * 2]), OutMsg);

        exit = 0;
        break;
      default:
        sendMessage(&(queue[idCasa]), InMsg);
        break;
      }
      break;
    default:
      break;
    }
  }

  pthread_join(mov_pid, NULL);
  pthread_join(humo_pid, NULL);
  return NULL;
}

static void *pRegado(void *arg) {
  REGADO_ESTADOS state;
  msg_t InMsg;
  state = IdleRegado;

  InMsg = receiveMessage(&(queue[REGADO]));
  double tiempo_regado = 0;
  int zona = *(int *)arg;

  switch (state) {
  case IdleRegado:
    switch (InMsg.signal) {
    case sTiempoRegado:
      tiempo_regado = InMsg.value;
      printf("\t\t\t Activando aspersores por %d sec en zona %d.\n",
             (int)tiempo_regado, zona);
      fflush(stdout);

      sleep((unsigned int)tiempo_regado);

      printf("\t\t\t Desactivando aspersores en zona %d.\n", zona);
      fflush(stdout);
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
  return NULL;
}

static void *pSistema(void *arg) {
  SISTEMA_ESTADOS state, state_next;
  msg_t InMsg, OutMsg;
  state_next = IdleSistema;
  pthread_t regado_tid, peatonal_pid, parqueadero_pid;
  pthread_t pCasas[NUM_CASAS + 1];
  pthread_t pHumedad[NUM_SENSOR_HUMEDAD + 1];
  int id_casa_respuesta, puerta_peatonal = -1, puerta_parqueadero = -1,
                         regando_activo = -1;

  // Instancias de Casas
  for (int index = 1; index <= NUM_CASAS; index++) {
    pthread_create(&pCasas[index], NULL, pControladorCasa, (void *)&index);
    sleep(1);
  }

  // Instancias de Sensores de Humedad
  for (int index = 1; index <= NUM_SENSOR_HUMEDAD; index++) {
    pthread_create(&pHumedad[index], NULL, pSensorHumedad, (void *)&index);
    sleep(1);
  }

  double tarjetasValidas[4] = {1, 2, 3, 4};
  char placasValidas[5][7] = {"JFU864", "AAA000", "AAA001", "BBB000", "BBB001"};
  int tarjetaValida = 0, placaValida = 0;
  int exit = 1;

  while (exit) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SISTEMA]));
    switch (state) {
    case IdleSistema:
      switch (InMsg.signal) {
      case sExitSistema:
        OutMsg.signal = sExitCasa;
        for (int index = 1; index <= NUM_CASAS; index++)
          sendMessage(&(queue[index]), OutMsg);

        OutMsg.signal = sExitHumedad;
        for (int index = 1; index <= NUM_SENSOR_HUMEDAD; index++)
          sendMessage(&(queue[10 + index - 1]), OutMsg);
        exit = 0;
        break;
      case sSolicitud:
        printf("\t\t\t Sistema -> Casa %d\n", InMsg.sender);
        fflush(stdout);
        OutMsg.signal = sNotificarCasa;
        sendMessage(&(queue[InMsg.sender]), OutMsg);

        id_casa_respuesta = InMsg.sender;
        state_next = EsperandoConfirmacion;
        break;
      case sLlegaCarroSistema:
        printf("\t\t\t Prendiendo camara parqueadero.\n");
        fflush(stdout);
        state_next = EsperandoFoto;
        break;
      case sLecturaRFID:
        for (int i = 0;
             i < sizeof(tarjetasValidas) / sizeof(tarjetasValidas[0]); i++) {
          if (tarjetasValidas[i] == InMsg.value) {
            tarjetaValida = 1;
            break;
          }
        }

        if (tarjetaValida) {
          tarjetaValida = 0;
          printf("\t\t\t Tarjeta %d ID Valida.\n", (int)InMsg.value);
          puerta_peatonal = pthread_create(&peatonal_pid, NULL, abrirPuerta,
                                           (void *)"peatonal");
        } else {
          printf("\t\t\t Tarjeta %d invalida.\n", (int)InMsg.value);
          fflush(stdout);
        }
        state_next = IdleSistema;
        break;
      case sIncendio:
        printf("\t\t\t Incendio en Casa %d\n", (int)InMsg.value);
        fflush(stdout);
        printf("\t\t\t Alertar vecinos de incendio\n");
        fflush(stdout);
        printf("\t\t\t Alertar bomberos de incendio\n");
        fflush(stdout);
        state_next = IdleSistema;
        break;
      case sNivelHumedadSistema:
        if (InMsg.value <= 0.5) {
          // Creando el proceso de regado
          regando_activo =
              pthread_create(&regado_tid, NULL, pRegado, (void *)&InMsg.sender);

          printf("\t\t\t Se tiene que activar los aspersores (Sistema -> "
                 "Regado)\n");
          fflush(stdout);

          OutMsg.signal = sTiempoRegado;
          OutMsg.value = 5 / InMsg.value;
          sendMessage(&(queue[REGADO]), OutMsg);
        } else {
          printf("\t\t\t No se necesita regar las plantas en zona %d.\n",
                 InMsg.sender);
          fflush(stdout);
        }
        state_next = IdleSistema;
        break;
      case sRobo:
        printf("\t\t\t Alertando Vecinos. (Sistema -> Casas)\n");
        fflush(stdout);
        printf("\t\t\t Alertando Policia.\n");
        fflush(stdout);
        id_casa_respuesta = (int)InMsg.value;
        state_next = EsperandoPolicia;
        break;
      default:
        break;
      }
      break;
    case EsperandoConfirmacion:
      switch (InMsg.signal) {
      case sInvitadoAcepta:
        if (InMsg.sender == id_casa_respuesta) {
          if (InMsg.value == 1) {
            printf("\t\t\t Invitado aceptado.\n");
            fflush(stdout);

            puerta_peatonal = pthread_create(&peatonal_pid, NULL, abrirPuerta,
                                             (void *)"peatonal");
          } else {
            printf("\t\t\t Invitado rechazado.\n");
            fflush(stdout);
          }
          state_next = IdleSistema;
        } else {
          printf("\t\t\t Casa %d No esta esperando visitas.\n", InMsg.sender);
          fflush(stdout);
        }
        break;
      case sExitSistema:
        OutMsg.signal = sExitCasa;
        for (int index = 1; index <= NUM_CASAS; index++)
          sendMessage(&(queue[index]), OutMsg);

        OutMsg.signal = sExitHumedad;
        for (int index = 1; index <= NUM_SENSOR_HUMEDAD; index++)
          sendMessage(&(queue[10 + index - 1]), OutMsg);

        exit = 0;
        break;
      default:
        sendMessage(&(queue[SISTEMA]), InMsg);
        break;
      }
      break;
    case EsperandoPolicia:
      switch (InMsg.signal) {
      case sLlegaPolicia:
        if (id_casa_respuesta == InMsg.sender) {
          printf("\t\t\t Llego policia (Sistema -> Casa %d)\n", InMsg.sender);
          fflush(stdout);

          OutMsg.signal = sRoboDetenido;
          sendMessage(&(queue[InMsg.sender]), OutMsg);
          state_next = IdleSistema;
        } else {
          printf("\t\t\t La casa %d no espera la policia.\n", InMsg.sender);
          fflush(stdout);
          state_next = EsperandoPolicia;
        }
        break;
      case sExitSistema:
        OutMsg.signal = sExitCasa;
        for (int index = 1; index <= NUM_CASAS; index++)
          sendMessage(&(queue[index]), OutMsg);

        OutMsg.signal = sExitHumedad;
        for (int index = 1; index <= NUM_SENSOR_HUMEDAD; index++)
          sendMessage(&(queue[10 + index - 1]), OutMsg);

        exit = 0;
        break;
      default:
        sendMessage(&(queue[SISTEMA]), InMsg);
        break;
      }
      break;
    case EsperandoFoto:
      switch (InMsg.signal) {
      case sLecturaCamara:
        printf("\t\t\t Enviando placa %s (Camara -> Sistema).\n", InMsg.placa);
        fflush(stdout);

        printf("\t\t\t Apagar camara.\n");
        fflush(stdout);

        for (int i = 0; i < sizeof(placasValidas) / sizeof(placasValidas[0]);
             i++) {
          if (strcmp(placasValidas[i], InMsg.placa) == 0) {
            placaValida = 1;
            break;
          }
        }
        if (placaValida) {
          placaValida = 0;
          puerta_parqueadero = pthread_create(
              &parqueadero_pid, NULL, abrirPuerta, (void *)"parqueadero");
        } else {
          printf("\t\t\t La placa es invalida.\n");
          fflush(stdout);
        }
        state_next = IdleSistema;
        break;
      case sExitSistema:
        OutMsg.signal = sExitCasa;
        for (int index = 1; index <= NUM_CASAS; index++)
          sendMessage(&(queue[index]), OutMsg);

        OutMsg.signal = sExitHumedad;
        for (int index = 1; index <= NUM_SENSOR_HUMEDAD; index++)
          sendMessage(&(queue[10 + index - 1]), OutMsg);

        exit = 0;
        break;
      default:
        sendMessage(&(queue[SISTEMA]), InMsg);
        break;
      }
      break;
    default:
      break;
    }
  }

  for (int index = 1; index <= NUM_CASAS; index++)
    pthread_join(pCasas[index], NULL);

  for (int index = 1; index <= NUM_SENSOR_HUMEDAD; index++)
    pthread_join(pHumedad[index], NULL);

  if (puerta_peatonal == 0)
    pthread_join(peatonal_pid, NULL);

  if (puerta_parqueadero == 0)
    pthread_join(parqueadero_pid, NULL);

  if (regando_activo == 0)
    pthread_join(regado_tid, NULL);

  return NULL;
}

/* Proceso auxiliar para la simulacion de abrir y cerrar puerta de la unidad */
static void *abrirPuerta(void *arg) {
  printf("\t\t\t Se abre la puerta %s por 5 sec.\n", (char *)arg);
  fflush(stdout);

  sleep(5);

  printf("\t\t\t Se cierra la puerta.\n");
  fflush(stdout);
  return NULL;
}
