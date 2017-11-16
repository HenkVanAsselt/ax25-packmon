/******************************************************

  FILE: CRC.H
  DESC: Headerfile for CRC calculations
  HIST: 940324 V0.1

*******************************************************/

WORD get_CRC(struct ax25_packet *packet);
int CRCcheck(struct ax25_level1 *packet);



