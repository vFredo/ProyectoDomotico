#include "phtrdsMsgLyr.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
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

int main(void) {
  pthread_t mov_tid, humo_tid, humedad_tid, foto_tid, rfid_tid;
  pthread_t sistema_tid, casa_tid;
  pthread_t panel_acc_tid, regado_tid;
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
  pthread_create(&regado_tid, NULL, pRegado, NULL);
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
  pthread_join(regado_tid, NULL);
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
    puts("1. Activar Sensor Movimiento de Casa\n"
         "2. On/Off Alarma Humo de Casa (Sensor Humo)\n"
         "3. Riego plantas (Sensor Humedad)\n"
         "4. Ingreso carro (Sensor Foto Electricto)\n"
         "5. Ingreso residente (Sensor RFID)\n"
         "6. Ingreso visitante (Panel Acceso)\n"
         "7. On/Off seguridad de Casa\n"
         "8. Exit");
    fflush(stdout);

    fflush(stdin);
    fgets(line, sizeof(line), stdin);
    sscanf(line, "%d", &choice);

    switch (choice) {
    case 1:
      printf("Enviando señal de movimiento al controlador de la casa...\n");
      fflush(stdout);

      OutMsg.signal = sMovimiento;
      OutMsg.value = 1;
      sendMessage(&(queue[SENSOR_MOVIMIENTO]), OutMsg);
      break;
    case 2:
      puts("\n0. Desactivar alarma humo\n"
           "1. Activar alarma humo");
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
      break;
    case 5:
      break;
    case 6:
      printf("Enviando peticion de entrada a la Casa del residente...\n");
      fflush(stdout);

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
        OutMsg.signal = sLlegaCarroSistema;
        OutMsg.value = InMsg.value;
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
        printf("\t\t\t Notificando al residente de una visita.\n");
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
        if (InMsg.value == 1) {
          printf("\t\t\t Alarma de humo activada.\n");
          fflush(stdout);
          alarmaHumo = 1;
        } else {
          printf("\t\t\t Alarma de humo desactivada.\n");
          fflush(stdout);
          alarmaHumo = 0;
        }
        state_next = IdleCasa;
        break;
      case sMovimientoCasa:
        if (seguridad_activa == 1) {
          printf("\t\t\t Alarma de Robo activada.\n");
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
        if (InMsg.value == 1) {
          printf("\t\t\t Seguridad activada.\n");
          fflush(stdout);
          seguridad_activa = 1;
        } else {
          printf("\t\t\t Seguridad desactivada.\n");
          fflush(stdout);
          seguridad_activa = 0;
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
        printf("\t\t\t Respuesta Residente: %lf.\n", InMsg.value);
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
  REGADO_ESTADOS state, state_next;
  msg_t InMsg, OutMsg;
  state_next = IdleRegado;

  double tiempo_regado = 0;
  int exit = 0;
  while (!exit) {
    state = state_next;
    InMsg = receiveMessage(&(queue[REGADO]));

    switch (state) {
    case IdleRegado:
      switch (InMsg.signal) {
      case sTiempoRegado:
        tiempo_regado = InMsg.value;
        printf("\t\t\t Activando aspersores por %lf sec.\n", tiempo_regado);
        fflush(stdout);
        state_next = Regando;
        break;
      default:
        break;
      }
      break;
    case Regando:
      sleep(tiempo_regado);
      printf("\t\t\t Desactivando aspersores.\n");
      fflush(stdout);
      exit = 1;
      break;
    default:
      break;
    }
  }
  return NULL;
}

static void *pSistema(void *arg) { return NULL; }
