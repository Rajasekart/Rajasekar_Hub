#pragma once
#include "afx.h"


class CFileCSV : public CStdioFile
{

public:

  enum Mode { modeRead, modeWrite };
  CFileCSV(LPCTSTR lpszFilename, Mode mode = modeRead);
  ~CFileCSV(void);

  bool ReadData(CStringArray &arr);
  void WriteData(CStringArray &arr);

#ifdef _DEBUG

  Mode m_nMode;

#endif


};