#ifndef bleCharas_i
#define bleCharas_i

#define BLE_CHARAS 4
#define IDX_EYES_X 0
#define IDX_EYES_Y 1
#define IDX_LID_POS 2
#define IDX_BLINK 3

#define RAW_LENGTH (BLE_CHARAS * sizeof(short))

typedef union chara_data_union
{
  uint8_t *raw;
  short *normal;
}
chara_data_t;

extern chara_data_t charaData;
extern chara_data_t charaData_sav;

#endif