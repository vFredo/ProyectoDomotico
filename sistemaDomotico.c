#include "phtrdsMsgLyr.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int main(void) {
  pthread_t mov_tid, humo_tid, humedad_tid, foto_tid, rfid_tid;
  pthread_t sistema_tid, casa_tid;
  pthread_t panel_acc_tid;
  pthread_t env_tid;

  system("clear");
  puts("Environment:                     Sistema Residencial Domotico\n"
       "-------------------------------------------------------------\n");

  /* Create queues */
  initialiseQueues();

  /* Create threads */
  pthread_create(&mov_tid, NULL, pSensorMovimiento, NULL);
  pthread_create(&humo_tid, NULL, pSensorHumo, NULL);
  pthread_create(&humedad_tid, NULL, pSensorHumedad, NULL);
  pthread_create(&foto_tid, NULL, pSensorFotoelectrico, NULL);
  pthread_create(&rfid_tid, NULL, pSensorRFID, NULL);
  pthread_create(&panel_acc_tid, NULL, pPanelAcceso, NULL);
  pthread_create(&sistema_tid, NULL, pSistema, NULL);
  pthread_create(&casa_tid, NULL, pControladorCasa, NULL);
  pthread_create(&env_tid, NULL, pEnvironment, NULL);

  /* Wait for threads to finish */
  pthread_join(mov_tid, NULL);
  pthread_join(humo_tid, NULL);
  pthread_join(humedad_tid, NULL);
  pthread_join(foto_tid, NULL);
  pthread_join(rfid_tid, NULL);
  pthread_join(panel_acc_tid, NULL);
  pthread_join(sistema_tid, NULL);
  pthread_join(casa_tid, NULL);
  pthread_join(env_tid, NULL);

  /* Destroy queues */
  destroyQueues();
  return 0;
}

/***( SDL system processes )**********************************************/

static void *pEnvironment(void *arg) {
  char line[100];
  int choice;
  msg_t OutMsg;

  for (;;) {
    puts("\n1. Activar Sensor Movimiento de Casa\n"
         "2. Nivel de Humo (Sensor Humo)\n"
         "3. Riego plantas (Sensor Humedad)\n"
         "4. Ingreso carro (Sensor Foto Electricto)\n"
         "5. Ingreso residente (Sensor RFID)\n"
         "6. Ingreso visitante (Panel Acceso)\n"
         "7. On/Off seguridad de Casa\n"
         "8. Llega policia a la unidad Residencial\n"
         "9. Exit");
    fflush(stdout);

    fflush(stdin);
    fgets(line, sizeof(line), stdin);
    sscanf(line, "%d", &choice);

    switch (choice) {
    case 1:
      OutMsg.signal = sMovimiento;
      OutMsg.value = 1;
      sendMessage(&(queue[SENSOR_MOVIMIENTO]), OutMsg);
      break;
    case 2:
      printf("Digite un valor para el nivel de humo entre 0.0 y 1.0:\n");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%lf", &OutMsg.value);

      OutMsg.signal = sNivelHumo;
      sendMessage(&(queue[SENSOR_HUMO]), OutMsg);
      break;
    case 3:
      printf("Digite un valor para la humedad de 0.0 a 1.0:\n");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%lf", &OutMsg.value);

      OutMsg.signal = sNivelHumedad;
      sendMessage(&(queue[SENSOR_HUMEDAD]), OutMsg);
      break;
    case 4:
      OutMsg.signal = sLlegaCarro;
      sendMessage(&(queue[SENSOR_FOTOELECTRICO]), OutMsg);

      printf("Digite la placa del carro:\n");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%s", OutMsg.placa);

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
      OutMsg.signal = sVisistanteSoliticaAcceso;
      OutMsg.value = 1;
      sendMessage(&(queue[PANEL_ACCESO]), OutMsg);

      sleep(1);
      puts("\nComo residente puedes:\n"
           "0. Rechazar Invitado.\n"
           "1. Aceptar Invitado.");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%lf", &OutMsg.value);

      OutMsg.signal = sRespuestaResidente;
      sendMessage(&(queue[CASA]), OutMsg);
      break;
    case 7:
      puts("\n0. Desactivar seguridad\n"
           "1. Activar seguridad");
      fflush(stdout);

      fflush(stdin);
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%lf", &OutMsg.value);

      OutMsg.signal = sSeguridadActiva;
      sendMessage(&(queue[CASA]), OutMsg);
      break;
    case 8:
      OutMsg.signal = sLlegaPolicia;
      sendMessage(&(queue[SISTEMA]), OutMsg);
      break;
    case 9:
      exit(0);
      break;
    default:
      printf("Opcion no valida. Intente de nuevo...\n\n");
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

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SENSOR_MOVIMIENTO]));
    switch (state) {
    case IdleMov:
      switch (InMsg.signal) {
      case sMovimiento:
        printf("\t\t\t Sensor Movimiento -> Casa.\n");
        fflush(stdout);
        OutMsg.signal = sMovimientoCasa;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[CASA]), OutMsg);
        state_next = IdleMov;
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

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SENSOR_HUMO]));
    switch (state) {
    case IdleHumo:
      switch (InMsg.signal) {
      case sNivelHumo:
        printf("\t\t\t Sensor Humo -> Casa.\n");
        fflush(stdout);
        OutMsg.signal = sNivelHumoCasa;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[CASA]), OutMsg);
        state_next = IdleHumo;
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
  msg_t InMsg, OutMsg;
  state_next = IdleHumedad;

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SENSOR_HUMEDAD]));
    switch (state) {
    case IdleHumedad:
      switch (InMsg.signal) {
      case sNivelHumedad:
        printf("\t\t\t Sensor Humo -> Sistema.\n");
        fflush(stdout);
        OutMsg.signal = sNivelHumedadSistema;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[SISTEMA]), OutMsg);
        state_next = IdleHumedad;
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

  for (;;) {
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

  for (;;) {
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

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[PANEL_ACCESO]));
    switch (state) {
    case IdlePanel:
      switch (InMsg.signal) {
      case sVisistanteSoliticaAcceso:
        printf("\t\t\t Panel de Acceso -> Sistema.\n");
        fflush(stdout);
        OutMsg.signal = sSolicitud;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[SISTEMA]), OutMsg);
        state_next = IdlePanel;
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

  state_next = IdleCasa;
  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[CASA]));
    switch (state) {
    case IdleCasa:
      switch (InMsg.signal) {
      case sNotificarCasa:
        printf("\t\t\t Notificando al residente de una visita.\n"
               "\t\t\t Casa -> Residente\n");
        fflush(stdout);
        state_next = EsperandoRespuestaResidente;
        break;
      case sRoboDetenido:
        printf("\t\t\t Alarma de Robo desactivada.\n");
        fflush(stdout);
        alarmaRobo = 0;
        state_next = IdleCasa;
        break;
      case sNivelHumoCasa:
        if (InMsg.value >= 0.7) {
          printf("\t\t\t Alarma de humo activada. (Casa -> Sistema)\n");
          fflush(stdout);

          alarmaHumo = 1;
          OutMsg.signal = sIncendio;
          OutMsg.value = pthread_self();
          sendMessage(&(queue[SISTEMA]), OutMsg);
        } else {
          printf("\t\t\t Alarma de humo desactivada.\n");
          fflush(stdout);
          alarmaHumo = 0;
        }
        state_next = IdleCasa;
        break;
      case sMovimientoCasa:
        if (seguridad_activa == 1) {
          printf("\t\t\t Alarma de Robo activada. Casa -> Sistema\n");
          fflush(stdout);

          alarmaRobo = 1;
          OutMsg.signal = sRobo;
          OutMsg.value = pthread_self();
          sendMessage(&(queue[SISTEMA]), OutMsg);
        } else {
          printf("\t\t\t La seguridad esta desactivada.\n");
          fflush(stdout);
        }
        state_next = IdleCasa;
        break;
      case sSeguridadActiva:
        switch ((int)InMsg.value) {
        case 0:
          if (alarmaRobo == 1) {
            printf("\t\t\t No se puede desactivar, Alarma de robo activa.\n");
            fflush(stdout);
          } else {
            printf("\t\t\t Seguridad desactivada.\n");
            fflush(stdout);
            seguridad_activa = 0;
          }
          break;
        case 1:
          if (seguridad_activa != 1) {
            printf("\t\t\t Seguridad activada.\n");
            fflush(stdout);
            seguridad_activa = 1;
          } else {
            printf("\t\t\t La seguridad ya esta activada.\n");
            fflush(stdout);
          }
          break;
        default:
          printf("\t\t\t Opcion invalida.\n");
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
        printf("\t\t\t Casa -> Sistema.\n");
        fflush(stdout);

        OutMsg.signal = sInvitadoAcepta;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[SISTEMA]), OutMsg);
        state_next = IdleCasa;
        break;
      default:
        sendMessage(&(queue[CASA]), InMsg);
        break;
      }
      break;
    default:
      break;
    }
  }
  return NULL;
}

static void *pRegado(void *arg) {
  REGADO_ESTADOS state;
  msg_t InMsg, OutMsg;
  state = IdleRegado;

  InMsg = receiveMessage(&(queue[REGADO]));
  double tiempo_regado = 0;

  switch (state) {
  case IdleRegado:
    switch (InMsg.signal) {
    case sTiempoRegado:
      tiempo_regado = InMsg.value;
      printf("\t\t\t Activando aspersores por %d sec.\n", (int)tiempo_regado);
      fflush(stdout);

      sleep((unsigned int)tiempo_regado);

      printf("\t\t\t Desactivando aspersores.\n");
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

  double tarjetasValidas[4] = {1, 2, 3, 4};
  char placasValidas[5][7] = {"JFU864", "AAA000", "AAA001", "BBB000", "BBB001"};
  int tarjetaValida = 0, placaValida = 0;

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SISTEMA]));
    switch (state) {
    case IdleSistema:
      switch (InMsg.signal) {
      case sSolicitud:
        printf("\t\t\t Sistema -> Casa\n");
        fflush(stdout);
        OutMsg.signal = sNotificarCasa;
        sendMessage(&(queue[CASA]), OutMsg);
        state_next = EsperandoConfirmacion;
        break;
      case sLlegaCarroSistema:
        printf("\t\t\t Prendiendo camara parqueadero.\n");
        fflush(stdout);
        state_next = EsperandoFoto;
        break;
      case sLecturaRFID:
        for (int i = 0; i < sizeof(tarjetasValidas) / sizeof(tarjetasValidas[0]); i++) {
          if (tarjetasValidas[i] == InMsg.value) {
            tarjetaValida = 1;
            break;
          }
        }

        if (tarjetaValida) {
          tarjetaValida = 0;
          printf("\t\t\t Tarjeta ID Valida.\n");
          pthread_create(&peatonal_pid, NULL, abrirPuerta, (void *)"peatonal");
        } else {
          printf("\t\t\t Tarjeta invalida.\n");
          fflush(stdout);
        }
        state_next = IdleSistema;
        break;
      case sIncendio:
        printf("\t\t\t Alertar vecinos de incendio\n");
        fflush(stdout);
        printf("\t\t\t Alertar bomberos de incendio\n");
        fflush(stdout);
        state_next = IdleSistema;
        break;
      case sNivelHumedadSistema:
        if (InMsg.value <= 0.5) {
          // Creando el proceso de regado
          pthread_create(&regado_tid, NULL, pRegado, NULL);

          printf("\t\t\t Se tiene que activar los aspersores (Sistema -> "
                 "Regado)\n");
          fflush(stdout);

          OutMsg.signal = sTiempoRegado;
          OutMsg.value = 5 / InMsg.value;
          sendMessage(&(queue[REGADO]), OutMsg);
        } else {
          printf("\t\t\t No se necesita regar las plantas.\n");
          fflush(stdout);
        }
        state_next = IdleSistema;
        break;
      case sRobo:
        printf("\t\t\t Alertando Vecinos. (Sistema -> Casa)\n");
        fflush(stdout);
        printf("\t\t\t Alertando Policia.\n");
        fflush(stdout);
        state_next = EsperandoPolicia;
        break;
      default:
        break;
      }
      break;
    case EsperandoConfirmacion:
      switch (InMsg.signal) {
      case sInvitadoAcepta:
        fflush(stdout);
        if (InMsg.value == 1) {
          printf("\t\t\t  Invitado aceptado.\n");
          fflush(stdout);

          pthread_create(&peatonal_pid, NULL, abrirPuerta, (void *)"peatonal");
        } else {
          printf("\t\t\t Invitado rechazado.\n");
          fflush(stdout);
        }
        state_next = IdleSistema;
        break;
      default:
        sendMessage(&(queue[SISTEMA]), InMsg);
        break;
      }
      break;
    case EsperandoPolicia:
      switch (InMsg.signal) {
      case sLlegaPolicia:
        printf("\t\t\t Llego policia (Sistema -> Casa)\n");
        fflush(stdout);
        OutMsg.signal = sRoboDetenido;
        sendMessage(&(queue[CASA]), OutMsg);
        state_next = IdleSistema;
        break;
      default:
        sendMessage(&(queue[SISTEMA]), InMsg);
        break;
      }
      break;
    case EsperandoFoto:
      switch (InMsg.signal) {
      case sLecturaCamara:
        printf("\t\t\t Apagar camara.\n");
        fflush(stdout);

        for (int i = 0; i < sizeof(placasValidas) / sizeof(placasValidas[0]); i++) {
          if (strcmp(placasValidas[i], InMsg.placa) == 0) {
            placaValida = 1;
            break;
          }
        }
        if (placaValida) {
          placaValida = 0;
          pthread_create(&parqueadero_pid, NULL, abrirPuerta, (void *)"parqueadero");
        } else {
          printf("\t\t\t La placa es invalida.\n");
          fflush(stdout);
        }
        state_next = IdleSistema;
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
