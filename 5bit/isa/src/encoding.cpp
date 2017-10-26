/**************************************
* -+-+-+-+-+-+-+-+-+-+-+-+-
* - Predmet: ISA
* - Projekt: LDAP Server
* - Modul: BER kodovani
* - Datum: 2017-09-26
* - Autor: Tomas Aubrecht
* - Login: xaubre02
* -+-+-+-+-+-+-+-+-+-+-+-+-
***************************************/

#include "encoding.hpp"
#include <iostream>
#include <sstream>

MyLDAPMsgDecoder::MyLDAPMsgDecoder(unsigned const char *buff, int len)
{
    clear();
    decode(buff, len);
}

void MyLDAPMsgDecoder::clear(void)
{
    // inicializace promennych
    rc   = rc_protocolError;
    id   = -1;
    type = -1;
}
    
void MyLDAPMsgDecoder::decode(unsigned const char *buff, int len)
{
    // -----------------------------
    // ------- DEBUG - tisk --------
    std::cout << "Received <: ";
    for (int c = 0; c < len; c++)
    {
       std::cout << std::hex << int(buff[c]) << " ";
    }
    std::cout << ":>\n";
    // -----------------------------
    
    
    // LDAPMessage musi mit alespon 7 bytu (LDAPMsg Len MsgID Len Val protocolOp Len)
    if (len < 7)
        return;

    // Musi se jednat o LDAPMessage a musi mit spravnou delku
    if (buff[0] != 0x30 || buff[1] != (len - 2))
        return;

    // MessageID musi mit typ INTEGER a nesmi mit nulovou delku, zaroven kontrola delky
    if (buff[2] != 0x02 || buff[3] == 0x00 || buff[3] > (len - 4))
        return;

    // kontrola delky daneho typu
    if (buff[6] != (len - 7))
        return;

    // ulozeni id
    id = buff[4];
    
    // ulozeni typu
    type = buff[5];

    switch (type)
    {
        case ldap_bind:
            // version musi byt typu INTEGER o delce 1 v rozsahu 0...127, ale podpora je pouze po verzi 3
            if (buff[7] != 0x02 || buff[8] != 0x01 || buff[9] > 0x03 || buff[9] < 0x01)
                return;

            // name musi byt typu OCTET STRING s nulovou délkou
            if (buff[10] != 0x04 || buff[11] != 0x00)
                return;
                
            // authentication simple - [0] OCTET STRING
            if (buff[12] != 0x80)
            {
                rc = rc_authMethodNotSupported;
                return;
            }
            
            // nulova delka OCTET STRINGu
            if (buff[13] != 0x00)
                return;
                
            break;
        
        case ldap_search:
            break;
        
        case ldap_unbind:
            if (buff[6] != 0x00)
                return;
                
            break;
        
        default:
            return;
    }
    
    // zprava byla uspesne dekodovana
    rc = rc_success;
}

int MyLDAPMsgDecoder::resultCode(void)
{
    return rc;
}

int MyLDAPMsgDecoder::getType(void)
{
    return type;
}

int MyLDAPMsgDecoder::getID(void)
{
    return id;
}

std::string MyLDAPMsgConstructor::createBindResponse(int msg_id, int res_code)
{
    std::stringstream msg;
    
    msg << (unsigned char)(0x30);    // ---------- LDAPMessage
    msg << (unsigned char)(0x0c);    // ---------- LENGTH
    msg << (unsigned char)(0x02);    // ----- MessageID
    msg << (unsigned char)(0x01);    // ----- LENGTH
    msg << (unsigned char)(msg_id);  // ----- VALUE
    msg << (unsigned char)(0x61);    // ----- bindResponse - [APPLICATION 1] SEQUENCE
    msg << (unsigned char)(0x07);    // ----- LENGTH
    msg << (unsigned char)(0x0a);    // ENUMERATE
    msg << (unsigned char)(0x01);    // LENGTH
    msg << (unsigned char)(res_code);// VALUE
    msg << (unsigned char)(0x04);    // matchedDN
    msg << (unsigned char)(0x00);    // LENGTH
    msg << (unsigned char)(0x04);    // diagnosticMessage
    msg << (unsigned char)(0x00);    // LENGTH
    
    return msg.str();
}

std::string MyLDAPMsgConstructor::createSearchResultDone(int msg_id, int res_code)
{
    std::stringstream msg;
    
    msg << (unsigned char)(0x30);    // ---------- LDAPMessage
    msg << (unsigned char)(0x0c);    // ---------- LENGTH
    msg << (unsigned char)(0x02);    // ----- MessageID
    msg << (unsigned char)(0x01);    // ----- LENGTH
    msg << (unsigned char)(msg_id);  // ----- VALUE
    msg << (unsigned char)(0x65);    // ----- searchResDone - [APPLICATION 5] SEQUENCE
    msg << (unsigned char)(0x07);    // ----- LENGTH
    msg << (unsigned char)(0x0a);    // ENUMERATE
    msg << (unsigned char)(0x01);    // LENGTH
    msg << (unsigned char)(res_code);// VALUE
    msg << (unsigned char)(0x04);    // matchedDN
    msg << (unsigned char)(0x00);    // LENGTH
    msg << (unsigned char)(0x04);    // diagnosticMessage
    msg << (unsigned char)(0x00);    // LENGTH
    
    return msg.str();
}

std::string MyLDAPMsgConstructor::createNoticeOfDisconnection(int resultCode)
{/*
    unsigned char msg[31];
    msg[0] = 0x30; // LDAPMessage
    msg[1] = 0x1d; // LENGTH
    msg[2] = 0x02; // INTEGER
    msg[3] = 0x01; // LENGTH
    msg[4] = 0x00; // VALUE
    msg[5] = 0x78; // extendedResp
    msg[6] = 0x18; // LENGTH
    msg[7] = 0x0a; // LDAPResult
    msg[8] = 0x01; // LENGTH
    msg[9] = resultCode; // VALUE 
    msg[10] = 0x04; // matchedDN
    msg[11] = 0x00; // LENGTH
    msg[12] = 0x04; // diagnosticMessage
    msg[13] = 0x00; // LENGTH
    msg[14] = 0x8a; // [10] LDAPOID
    msg[15] = 0x0f; // LENGTH
    msg[16] = 0x01; // 1
    msg[17] = 0x2e; // .
    msg[18] = 0x03; // 3
    msg[19] = 0x2e; // .
    msg[20] = 0x06; // 6
    msg[21] = 0x2e; // .
    msg[22] = 0x01; // 1
    msg[23] = 0x2e; // .
    msg[24] = 0x04; // 4
    msg[25] = 0x2e; // .
    msg[26] = 0x01; // 1
    msg[27] = 0x2e; // .
    msg[28] = 0x0; // 1466
    msg[29] = 0x2e; // .
    msg[30] = 0x0; // 20036*/
    resultCode++;
    return std::string("faaail");
}
