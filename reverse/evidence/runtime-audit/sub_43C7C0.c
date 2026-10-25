int __fastcall sub_43C7C0(
        int a1,
        unsigned __int8 *a2,
        int *a3,
        int *a4,
        int a5,
        int a6,
        int a7,
        int a8,
        _DWORD *a9,
        int a10)
{
  char *v11; // ebx
  int v12; // eax
  _BYTE *v13; // esi
  int Language; // ecx
  int v15; // edi
  const CHAR *v16; // eax
  char v17; // dl
  char *i; // ecx
  char v19; // al
  bool v20; // zf
  char v21; // al
  int v22; // ecx
  int v23; // esi
  int v24; // edi
  unsigned __int8 v25; // cl
  char *v26; // edi
  char *v27; // esi
  unsigned __int8 v28; // al
  unsigned __int8 v29; // dl
  char *v30; // edi
  unsigned __int8 v31; // dh
  unsigned __int8 v32; // cl
  int v33; // eax
  int v34; // edi
  LPCSTR v35; // ecx
  char *v36; // esi
  char v37; // al
  unsigned int v38; // ebx
  __int16 v39; // dx
  __int16 v40; // si
  int v41; // edi
  CHAR v42; // cl
  int v43; // ebx
  BOOL v44; // eax
  char *j; // esi
  int v46; // eax
  int v47; // eax
  int v48; // esi
  int v49; // edx
  int v50; // esi
  int v51; // eax
  int v52; // ecx
  char *v53; // ebx
  int v54; // eax
  char *v55; // edx
  const CHAR *k; // eax
  const CHAR *v57; // esi
  int v58; // edi
  int v59; // eax
  int v60; // ecx
  int v61; // edx
  int v62; // eax
  int v63; // eax
  int v64; // edi
  char *v65; // esi
  int v66; // edx
  int v67; // eax
  char *v68; // eax
  int v69; // eax
  unsigned __int8 v70; // dl
  char v71; // dl
  int v72; // ecx
  char *v73; // eax
  int v74; // edx
  int v75; // eax
  int v76; // ebx
  int v78; // [esp+10h] [ebp-10A0h]
  char *v79; // [esp+10h] [ebp-10A0h]
  int v80; // [esp+14h] [ebp-109Ch]
  int v81; // [esp+14h] [ebp-109Ch]
  char v82; // [esp+1Bh] [ebp-1095h]
  LPCSTR lpString; // [esp+1Ch] [ebp-1094h]
  const CHAR *lpStringa; // [esp+1Ch] [ebp-1094h]
  LPCSTR lpStringb; // [esp+1Ch] [ebp-1094h]
  __int16 v86; // [esp+20h] [ebp-1090h] BYREF
  int v87; // [esp+24h] [ebp-108Ch]
  int v88; // [esp+28h] [ebp-1088h]
  int v89; // [esp+2Ch] [ebp-1084h]
  int v90; // [esp+30h] [ebp-1080h]
  int v91; // [esp+34h] [ebp-107Ch]
  int v92; // [esp+38h] [ebp-1078h]
  char *Str1; // [esp+3Ch] [ebp-1074h]
  __int16 v94; // [esp+40h] [ebp-1070h]
  int v95; // [esp+44h] [ebp-106Ch]
  _DWORD *v96; // [esp+48h] [ebp-1068h]
  int v97; // [esp+4Ch] [ebp-1064h]
  char *Source; // [esp+50h] [ebp-1060h]
  int v99; // [esp+54h] [ebp-105Ch] BYREF
  int v100; // [esp+58h] [ebp-1058h] BYREF
  int v101; // [esp+5Ch] [ebp-1054h]
  int v102; // [esp+60h] [ebp-1050h]
  int v103; // [esp+64h] [ebp-104Ch]
  int v104; // [esp+68h] [ebp-1048h]
  int v105; // [esp+6Ch] [ebp-1044h]
  int v106; // [esp+70h] [ebp-1040h]
  int *v107; // [esp+74h] [ebp-103Ch]
  int v108; // [esp+78h] [ebp-1038h]
  int v109; // [esp+7Ch] [ebp-1034h]
  int v110; // [esp+80h] [ebp-1030h]
  int v111; // [esp+84h] [ebp-102Ch]
  int *v112; // [esp+88h] [ebp-1028h]
  _DWORD v113[4]; // [esp+8Ch] [ebp-1024h] BYREF
  CHAR Destination[12]; // [esp+9Ch] [ebp-1014h] BYREF
  _BYTE v115[4100]; // [esp+A8h] [ebp-1008h] BYREF

  v107 = a3;
  v97 = a8;
  v96 = a9;
  Str1 = (char *)a2;
  v102 = a1;
  v112 = a4;
  v105 = a10;
  if ( (sub_44AC00(a1) & 0x30) != 0 )
    return 0;
  v108 = 1;
  v86 = 0;
  v11 = v115;
  v101 = 0;
  if ( v107 )
    v78 = *v107;
  else
    v78 = 0;
  if ( a4 )
    v12 = *a4;
  else
    v12 = 0;
  v80 = v12;
  v13 = a2 + 1;
  v91 = 0;
  v90 = 0;
  PalDebugPrintf(0, 0, "AdvTextSetString2219 %d %d", *a2, a2[1]);
  Language = PalGetLanguage();
  v15 = 0;
  v16 = 0;
  v92 = Language;
  lpString = 0;
  if ( !Language )
    goto LABEL_18;
  while ( 1 )
  {
    v17 = *(v13 - 1);
    if ( !v17 )
      break;
    if ( v17 == 59 && *v13 == 32 )
    {
      PalDebugPrintf(0, 0, "[%d][%d]lpSrc[dwBufPoint] == cToken", Language, v16);
      v16 = lpString;
      Language = --v92;
      if ( !v92 )
        goto LABEL_17;
    }
LABEL_16:
    v16 += 2;
    v13 += 2;
    lpString = v16;
  }
  if ( *v13 )
    goto LABEL_16;
  if ( !Language )
  {
LABEL_17:
    v16 += 2;
LABEL_18:
    for ( i = &Str1[(_DWORD)v16]; ; i += 2 )
    {
      v19 = *i;
      if ( *i )
      {
        if ( v19 != 59 )
          goto LABEL_24;
        v20 = i[1] == 32;
      }
      else
      {
        v20 = i[1] == 0;
      }
      if ( v20 )
      {
        *(_WORD *)&v115[v15] = 0;
        goto LABEL_26;
      }
LABEL_24:
      v115[v15] = v19;
      v21 = i[1];
      v115[v15 + 1] = v21;
      v15 += 2;
    }
  }
  PalDebugPrintf(0, 0, "!LPBYTELanguageSplit");
  v11 = Str1;
LABEL_26:
  if ( v102 )
  {
    v101 = PalSpriteLock(v102, &v100, &v99);
  }
  else
  {
    v100 = a5;
    v99 = a6;
  }
  PalFontBegin();
  v89 = a7;
  if ( v97 )
  {
    v92 = v97 + 1;
    v22 = a7 + v97 + 3;
  }
  else
  {
    v92 = 0;
    v22 = a7 + 2;
  }
  v87 = v22;
  v106 = a7;
  v110 = v22;
  v88 = PalTaskGetTaskData(0) + 824;
  v82 = 0;
  Source = 0;
  *((_BYTE *)&word_54CED4 + v88) = 0;
  Str1 = 0;
LABEL_33:
  v23 = v80;
LABEL_34:
  v24 = v78;
LABEL_35:
  while ( 2 )
  {
    while ( 2 )
    {
      while ( 2 )
      {
        v25 = *v11;
        v103 = v24;
        v26 = v11;
        v111 = v23;
        v104 = (int)v11;
        if ( !v25 && !v11[1] )
        {
          v24 = v78;
          goto LABEL_157;
        }
        v27 = v11 + 1;
        PalDebugPrintf(0, 0, "AdvTextSetString2254 %d %d", v25, (unsigned __int8)v11[1]);
        v28 = *v11;
        if ( *v11 == 60 && !*v27 )
        {
          v29 = v11[2];
          v30 = v11 + 2;
          v31 = v11[4];
          v32 = v11[6];
          LOBYTE(v94) = *v11;
          v33 = *(_DWORD *)v11;
          HIBYTE(v94) = v29;
          lpStringa = v11 + 4;
          if ( v33 == 1047683644 )
          {
            v11 += 4;
            v24 = 0;
            v23 = v87 + v80;
            v78 = 0;
            v90 += a5 - (v87 + v80);
            v80 += v87;
            continue;
          }
          if ( v33 == 1852138300 && *((_DWORD *)v11 + 1) == 1047684468 )
          {
            v11 += 8;
            v34 = v89;
            v81 = v89;
            v35 = 0;
            v79 = v11;
            v36 = v11;
            v37 = *v11;
            if ( *v11 )
            {
              v38 = 0;
              while ( 1 )
              {
                v39 = *(_WORD *)v36;
                if ( v37 == 60 )
                {
                  if ( *(_WORD *)v36 == 29500 )
                  {
                    v34 = sub_44E460(v36 + 2);
                    v36 += v109 + 3;
                    if ( v34 > v81 )
                      v81 = v34;
                  }
                  else
                  {
                    v36 = strstr(v36, ">") + 1;
                  }
                }
                else
                {
                  v38 += v34;
                  v36 += 2;
                  if ( v38 > a5 && (!sub_43B9B0(((char)v39 << 8) | SHIBYTE(v39)) || v38 > v34 + a5) )
                  {
LABEL_54:
                    lpStringb = (LPCSTR)v38;
                    v11 = v79;
                    v35 = lpStringb;
                    break;
                  }
                }
                v37 = *v36;
                if ( !*v36 )
                  goto LABEL_54;
              }
            }
            v23 = ((a6 - v81) >> 1) - v97 - v92;
            v24 = (a5 - (int)v35) >> 1;
            v90 += v24;
            v78 = v24;
            v80 = v23;
            if ( v105 )
              *(_DWORD *)(v105 + 20) = a6;
            continue;
          }
          v40 = v94;
          if ( v94 == 29500 )
          {
            v41 = 0;
            v91 = 0;
            lstrlenA(lpStringa);
            v42 = *lpStringa;
            v43 = 0;
            v95 = 0;
            if ( v42 == 45 )
            {
              v91 = 1;
              v44 = 1;
            }
            else
            {
              v44 = v42 == 43;
            }
            for ( j = (char *)&lpStringa[v44]; *j || j[1]; j += 2 )
            {
              PalDebugPrintf(0, 0, "[%d]lpStr", *j);
              if ( (unsigned __int8)(*j - 48) > 9u )
                break;
              PalDebugPrintf(0, 0, "[%d]lResult", v41);
              v46 = *j;
              v43 += 2;
              v41 = v46 + 2 * (5 * v41 - 24);
            }
            v95 = v43;
            v47 = -v41;
            if ( !v91 )
              v47 = v41;
            v48 = (int)((double)v47 * 1.5);
            PalDebugPrintf(0, 0, "[%d]fontsize", v48);
            PalFontEnd();
            PalFontSetFontSize(v48);
            PalFontBegin();
            v49 = 0;
            v11 = (char *)&lpStringa[v43 + 2];
            if ( v89 - v48 >= 0 )
              v49 = v89 - v48;
            v91 = v49;
            if ( v48 > v89 )
            {
              v87 += v48 - v89;
              if ( v105 )
                *(_DWORD *)(v105 + 20) += v48 - v89;
            }
            v89 = v48;
            goto LABEL_33;
          }
          v104 = ((v29 | ((v31 | (v32 << 8)) << 8)) << 8) | 0x3C;
          if ( v104 == 1047736124 )
          {
            v11 += 8;
            PalFontEnd();
            v50 = v106;
            PalFontSetFontSize(v106);
            PalFontBegin();
            v89 = v50;
            v91 = 0;
            goto LABEL_33;
          }
          if ( !strncmp(v11, dword_4A28BC, 7u) )
          {
            v11 += 7;
            v87 = v110;
LABEL_78:
            PalFontSetColor(*v96, v96[1]);
            goto LABEL_33;
          }
          if ( v40 == 29244 )
          {
            PalFontEnd();
            v51 = sub_43BD90(0, v11, 0, 0, 0, 0, v97);
            v52 = v51 + v78;
            v95 = v51;
            if ( v51 + v78 > a5 )
              v90 += v51;
            v53 = v11 + 2;
            v23 = v87 + v80;
            if ( v52 <= a5 )
              v23 = v111;
            v54 = 0;
            v80 = v23;
            if ( v52 <= a5 )
              v54 = v103;
            v55 = v30;
            v24 = v54;
            v78 = v54;
            v11 = &v53[sub_43BD90(v101, v55, v54, v23, v100, v99, v97)];
            PalFontBegin();
            continue;
          }
          if ( v40 == 25404 )
          {
            v11 += 4;
            v103 = v96[1];
            for ( k = lpStringa; ; k += 2 )
            {
              while ( !*k )
              {
                v23 = v80;
                v24 = v78;
                if ( !k[1] )
                  goto LABEL_35;
                k += 2;
              }
              if ( *k == 35 && !k[1] )
                break;
            }
            v57 = k + 1;
            *(_DWORD *)Destination = 30768;
            strncat(&Destination[2], k + 1, 6u);
            v58 = sub_44E370(Destination);
            while ( 1 )
            {
              if ( *v57 )
              {
                if ( *v57 == 35 && !v57[1] )
                {
                  *(_DWORD *)Destination = 30768;
                  strncpy(&Destination[2], v57 + 1, 6u);
                  v59 = sub_44E370(Destination);
LABEL_99:
                  PalFontSetColor(v58, v59);
                  while ( 2 )
                  {
                    if ( *v11 )
                    {
                      if ( *v11 == 62 && !v11[1] )
                        goto LABEL_103;
LABEL_110:
                      v11 += 2;
                      continue;
                    }
                    break;
                  }
                  if ( v11[1] )
                    goto LABEL_110;
                  v11 = 0;
LABEL_103:
                  v11 += 2;
                  goto LABEL_33;
                }
              }
              else if ( !v57[1] )
              {
                v59 = v103;
                goto LABEL_99;
              }
              v57 += 2;
            }
          }
          if ( v104 == 1046687548 )
          {
            v11 += 8;
            goto LABEL_78;
          }
          if ( *(_WORD *)v11 != 25660 )
          {
            if ( *(_DWORD *)v11 != 1046753084 )
            {
              v11 += sub_43BCC0(v11, ">") + 2;
              goto LABEL_33;
            }
            if ( *(_DWORD *)(v88 + 88448) )
            {
              v23 = v80;
            }
            else
            {
              v64 = 1547 * *(int *)((char *)&dword_54CEC8 + v88);
              v82 = 0;
              v65 = (char *)&unk_54CF05 + v88 + v64 * 4;
              memset(v65, 0, 0x7FFu);
              memset(&byte_54D704[v88 + v64 * 4], 0, 0x7FFu);
              memset(&byte_54DF03[v88 + v64 * 4], 0, 0x7FFu);
              strncpy(v65, Source, v11 - Source);
              v66 = v88;
              v23 = v80;
              v67 = v80 + v87;
              if ( byte_54CF04[v64 * 4 + v88] )
              {
                *(int *)((char *)&dword_54CEF8[v64] + v88) = v67;
                v68 = Str1;
                *(int *)((char *)&dword_54CEF4[v64] + v66) = v78;
                strncpy(&byte_54D704[v66 + v64 * 4], Source, v68 - Source);
                strncpy(&byte_54DF03[v88 + v64 * 4], Str1, v11 - Str1);
                v66 = v88;
              }
              else
              {
                *(int *)((char *)&dword_54CEE4[v64] + v88) = v78;
                *(int *)((char *)&dword_54CEE8[v64] + v66) = v67;
              }
              *(int *)((char *)&dword_54CEFC[v64] + v66) = v89;
              *(int *)((char *)&dword_54CF00[v64] + v66) = v87;
              v69 = ++*(int *)((char *)&dword_54CEC8 + v66);
              *(__int16 *)((char *)&word_54CED4 + v66) = 257;
              if ( v69 >= 32 )
                v69 = 1;
              v20 = byte_57D459[v66] == 0;
              *(int *)((char *)&dword_54CEC8 + v66) = v69;
              if ( !v20 )
              {
                PalFontSetColor(*v96, v96[1]);
                v11 += 4;
                goto LABEL_34;
              }
            }
            v11 += 4;
            goto LABEL_34;
          }
          v60 = -1;
          if ( (unsigned __int8)(*v30 - 48) <= 9u )
            v60 = sub_44E460(v11 + 2);
          while ( 1 )
          {
            if ( *v30 )
            {
              if ( *v30 == 62 && !v30[1] )
                goto LABEL_119;
            }
            else if ( !v30[1] )
            {
              v30 = 0;
LABEL_119:
              v61 = v88;
              v11 = v30 + 1;
              v23 = v80;
              v24 = v78;
              if ( !*(_DWORD *)(v88 + 88448) )
              {
                v62 = *(int *)((char *)&dword_54CEC8 + v88);
                if ( v62 == 31 )
                {
                  *(int *)((char *)&dword_54CEC8 + v88) = 0;
                  v62 = 0;
                }
                v24 = v78;
                v23 = v80;
                v63 = 1547 * v62;
                v82 = 1;
                Source = v11;
                *(int *)((char *)&dword_54CED8[v63] + v61) = v60;
                *(int *)((char *)&dword_54CEDC[v63] + v61) = v78;
                *(int *)((char *)&dword_54CEE0[v63] + v61) = v80;
                byte_54CF04[v63 * 4 + v61] = 0;
                if ( byte_57D459[v61] )
                  PalFontSetColor(*(int *)((char *)&dword_57D45C + v61), *(int *)((char *)&dword_57D460 + v61));
              }
              goto LABEL_35;
            }
            v30 += 2;
          }
        }
        break;
      }
      v11 += 2;
      LOBYTE(v86) = v28;
      v70 = *v27;
      HIBYTE(v86) = *v27;
      if ( v78 < a5 || sub_43B9B0(v70 | (v28 << 8)) && (v26 = (char *)v104, v78 - 2 + v89 <= v89 + a5) )
      {
        v24 = v78;
        v23 = v80;
      }
      else
      {
        v72 = 1547 * *(int *)((char *)&dword_54CEC8 + v88);
        if ( v82 )
        {
          v20 = v71 == 0;
          v73 = v27;
          v74 = v88;
          if ( !v20 )
            v73 = v26;
          Str1 = v73;
          *(int *)((char *)&dword_54CEE4[v72] + v88) = v78;
          v75 = v87;
          v23 = v87 + v80;
          byte_54CF04[v72 * 4 + v74] = 1;
          *(int *)((char *)&dword_54CEE8[v72] + v74) = v23;
        }
        else
        {
          v75 = v87;
          v23 = v87 + v80;
          v74 = v88;
        }
        v24 = 0;
        v80 = v23;
        v78 = 0;
        if ( v23 + v75 > a6 )
        {
          v86 = 25473;
          goto LABEL_157;
        }
        if ( v86 == 16513 )
          continue;
        if ( v82 )
        {
          *(int *)((char *)&dword_54CEEC[v72] + v74) = 0;
          *(int *)((char *)&dword_54CEF0[v72] + v74) = v23;
        }
      }
      break;
    }
    if ( !v102 || (v108 = PalFontDrawText(v101, &v86, v24, v23 + v92 + v91, v100, v99)) != 0 )
    {
      v24 += v89;
      v90 += v89;
      v78 = v24;
      continue;
    }
    break;
  }
LABEL_157:
  PalFontEnd();
  v76 = v102;
  PalSpriteUnlock(v102);
  PalFontSetFontSize(v106);
  PalFontSetColor(*v96, v96[1]);
  v113[0] = 0;
  v113[2] = v89 + 2 + a5;
  v113[1] = 0;
  v113[3] = v23 + v87 + 2;
  PalSpriteSetRect(v76, v113);
  if ( v107 )
    *v107 = v24;
  if ( v112 )
    *v112 = v23;
  if ( v108 )
    return v90 + 64;
  return 0;
}