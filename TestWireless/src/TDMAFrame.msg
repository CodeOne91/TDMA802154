packet TDMAFrame
{
  string DestAddr;
  string SrcAddr;
  int    ackId;
  bool  isAck;
  int retransmissionCnt;
}
