#include "phtrdsMsgLyr.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>

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
static void *environment(void *arg);

int main(void) {
  pthread_t mov_tid;
  pthread_t humo_tid;
  pthread_t humedad_tid;
  pthread_t foto_tid;
  pthread_t rfid_tid;
  pthread_t regado_tid;
  pthread_t panelacc_tid;
  pthread_t sis_tid;
  pthread_t ctrlcasa_tid;
  pthread_t env_tid;

  /* Create queues */
  initialiseQueues();

  /* Create threads */
  pthread_create(&mov_tid, NULL, pSensorMovimiento, NULL);
  pthread_create(&humo_tid, NULL, pSensorHumo, NULL);
  pthread_create(&humedad_tid, NULL, pSensorHumedad, NULL);
  pthread_create(&foto_tid, NULL, pSensorFotoelectrico, NULL);
  pthread_create(&rfid_tid, NULL, pSensorRFID, NULL);
  pthread_create(&regado_tid, NULL, pRegado, NULL);
  pthread_create(&panelacc_tid, NULL, pPanelAcceso, NULL);
  pthread_create(&sis_tid, NULL, pSistema, NULL);
  pthread_create(&ctrlcasa_tid, NULL, pControladorCasa, NULL);
  pthread_create(&env_tid, NULL, environment, NULL);

  /* Wait for threads to finish */
  pthread_join(mov_tid, NULL);
  pthread_join(humo_tid, NULL);
  pthread_join(humedad_tid, NULL);
  pthread_join(foto_tid, NULL);
  pthread_join(rfid_tid, NULL);
  pthread_join(regado_tid, NULL);
  pthread_join(panelacc_tid, NULL);
  pthread_join(sis_tid, NULL);
  pthread_join(env_tid, NULL);

  /* Destroy queues */
  destroyQueues();
  return 0;
}

// NOTE: Hilo donde el usuario digita las señales de ambiente
static void *environment(void *arg) {
  char line[100];
  int choice;
  msg_t OutMsg;

  while (1) {
    puts("1. Sensor Movimiento\n"
         "2. Sensor Humo\n"
         "3. Sensor Humedad\n"
         "4. Sensor Foto Electricto\n"
         "5. Sensor RFID\n"
         "6. Panel Acceso\n"
         "7. Quit");
    fflush(stdout);
    fflush(stdin);

    fgets(line, sizeof(line), stdin);
    sscanf(line, "%d", &choice);
    switch (choice) {
    case 1:
      puts("Enviando señal de movimiento al sistema...");
      fflush(stdout);
      OutMsg.signal = sMovimiento;
      OutMsg.value = 1;
      sendMessage(&(queue[SENSOR_MOVIMIENTO_ENV]), OutMsg);
      break;
    case 2:
      puts("Digite el valor del humo 0 o 1\n");
      sscanf(line, "%lf", &OutMsg.value);
      fflush(stdout);
      fflush(stdin);
      OutMsg.signal = sNivelHumo;
      sendMessage(&(queue[SENSOR_HUMO_ENV]), OutMsg);
      break;
    case 3:
      puts("Digite el valor de la humedad 0 o 1\n");
      sscanf(line, "%lf", &OutMsg.value);
      fflush(stdout);
      fflush(stdin);
      OutMsg.signal = sNivelHumedad;
      sendMessage(&(queue[SENSOR_HUMEDAD_ENV]), OutMsg);
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    default:
      puts("Opcion no valida. Intente de nuevo.\n");
      fflush(stdout);
      break;
    }
  }

  return NULL;
}

/***( SDL system processes )**********************************************/
static void *pSensorMovimiento(void *arg) {
  SENSOR_MOVIENTO_ESTADOS state, state_next;
  msg_t InMsg, OutMsg;
  state_next = IdleM;

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SENSOR_MOVIMIENTO_ENV]));
    switch (state) {
    case IdleM:
      switch (InMsg.signal) {
      case sMovimiento:
        OutMsg.signal = sMovimientoCasa;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[CONTROLADOR_CASA]), OutMsg);
        state_next = IdleM;
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
  state_next = IdleH;

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SENSOR_HUMO_ENV]));
    switch (state) {
    case IdleH:
      switch (InMsg.signal) {
      case sNivelHumo:
        OutMsg.signal = sNivelHumoCasa;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[CONTROLADOR_CASA]), OutMsg);
        state_next = IdleH;
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
  state_next = IdleHu;

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SENSOR_HUMEDAD_ENV]));
    switch (state) {
    case IdleHu:
      switch (InMsg.signal) {
      case sNivelHumedad:
        OutMsg.signal = sNivelHumedadSistema;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[CONTROLADOR_SISTEMA]), OutMsg);
        state_next = IdleHu;
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
  state_next = IdleF;

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SENSOR_FOTOELECTRICO_ENV]));
    switch (state) {
    case IdleF:
      switch (InMsg.signal) {
      case sLlegaCarro:
        OutMsg.signal = sLlegaCarroSistema;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[CONTROLADOR_SISTEMA]), OutMsg);
        state_next = IdleF;
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
  state_next = IdleR;

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[SENSOR_RFID_ENV]));
    switch (state) {
    case IdleR:
      switch (InMsg.signal) {
      case sTarjetaEnSensor:
        OutMsg.signal = sLecturaRFID;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[CONTROLADOR_SISTEMA]), OutMsg);
        state_next = IdleR;
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
  state_next = IdleP;

  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[PANEL_ACCESO_ENV]));
    switch (state) {
    case IdleP:
      switch (InMsg.signal) {
      case sVisistanteSoliticaAcceso:
        OutMsg.signal = sSolicitud;
        OutMsg.value = InMsg.value;
        sendMessage(&(queue[CONTROLADOR_SISTEMA]), OutMsg);
        state_next = IdleP;
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
  SISTEMA_ESTADOS state, state_next;
  msg_t InMsg, OutMsg;
  msg_t respuesta_residente;
  int seguridad_activa;
  int alarmaRobo = 0;
  int alarmaHumo = 0;

  state_next = IdleS;
  for (;;) {
    state = state_next;
    InMsg = receiveMessage(&(queue[CONTROLADOR_CASA]));
    switch (state) {
    case IdleS:
      switch (InMsg.signal) {
      case sNotificarCasa:
        respuesta_residente = receiveMessage(&(queue[CONTROLADOR_CASA_ENV]));
        OutMsg.signal = sInvitadoAcepta;
        OutMsg.value = respuesta_residente.value;
        sendMessage(&(queue[CONTROLADOR_SISTEMA]), OutMsg);
        state_next = IdleS;
        break;
      case sRoboDetenido:
        // NOTE: informar al usuario de parar la alarma
        alarmaRobo = 0;
        state_next = IdleS;
        break;
      case sNivelHumoCasa:
        // NOTE: informar al usuario de parar la alarma
        if (InMsg.value >= 1) {
          alarmaHumo = 1;
        } else {
          alarmaHumo = 0;
        }
        state_next = IdleS;
        break;
      case sMovimientoCasa:
        // NOTE: informar al usuario de parar la alarma
        if (seguridad_activa == 1) {
          alarmaRobo = 1;
          OutMsg.signal = sRobo;
          OutMsg.value = pthread_self();
          sendMessage(&(queue[CONTROLADOR_SISTEMA]), OutMsg);
        } else {
          alarmaRobo = 0;
        }
        state_next = IdleS;
        break;
      case sSeguridadActiva:
        // NOTE: informar al usuario de parar la alarma
        if (InMsg.value == 1) {
          seguridad_activa = 1;
        } else {
          seguridad_activa = 0;
        }
        state_next = IdleS;
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

static void *pSistema(void *arg) { return NULL; }
static void *pRegado(void *arg) { return NULL; }
