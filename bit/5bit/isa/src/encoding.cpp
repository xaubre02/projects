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
#include <vector>

req_atts::req_atts(std::vector<std::string> atts)
{
    valid = true;
    // pokud je vstupni vector prazdny, implicitne se pozaduji vsechny atributy
    if (atts.empty())
    {
        cn = true;
        uid = true;
        mail = true;
        return;
    }
    
    // nastaveni priznaku
    for (auto str : atts)
    {
        if (!str.compare("cn") || !str.compare("CommonName"))
            cn = true;

        else if (!str.compare("uid") || !str.compare("UserID"))
            uid = true;

        else if (!str.compare("mail"))
            mail = true;

        else
        {
            // neznamy atribut
            valid = false;
            break;
        }
    }
}

/******************************************************************************/

csv_record::csv_record(std::string str)
{
    valid = false;
    // nalezeni prvniho oddelovace
    size_t pos = str.find(";");
    if (pos == std::string::npos)
        return;
    
    // ulozeni cn hodnoty
    cn = str.substr(0, pos++);
    
    // nalezeni druheho oddelovace
    size_t end = str.find(";", pos);
    if (pos == std::string::npos)
        return;
        
    // ulozeni uid a emailu
    uid = str.substr(pos, end - pos);
    mail = str.substr(end + 1);
    
    // odstraneni nul
    cn = customize(cn);
    uid = customize(uid);
    mail = customize(mail);
    
    // odstraneni konce radku
    mail = mail.substr(0, mail.length() - 1);
    valid = true;
}

std::string csv_record::customize(std::string str)
{
    std::string tmp;
    for (unsigned int c = 0; c < str.length(); c++)
    {
        // pokud to neni nula, pridej
        if(int(str.c_str()[c]) != 0)
            tmp.append(str.substr(c, 1));
    }
    return tmp;
}

/******************************************************************************/

bool IntegerBER::decode(const unsigned char *buff, int msg_len)
{
    // inicializace
    result = 0;
    len = 2;
    enc.str(std::string());
    
    // kontrola, zdali se jedna o typ integer
    if (buff[0] != 0x02)
        return false;
    
    // kontrola delky integeru - nemuze mit vice oktetu nez je poslano, mene nez jeden a zaroven musi byt v rozsahu 0...2147483647, což se vleze na 4 oktety
    if (buff[1] >= msg_len || buff[1] < 0x01 || buff[1] > 0x04)
        return false;
    
    // kontrola, zdali se nejedna o zaporne cislo - nejvyssi bit je 1
    if (buff[2] & 0x80)
        return false;
    
    // vysledna hodnota
    enc << buff[0]; // type INTEGER
    enc << buff[1]; // delka
    for (int i = 0; i < buff[1]; i++)
    {
        // podle celkoveho poctu oktetu se shiftuji jednotlive hodnoty o pozadovany pocet oktetu
        result += buff[2 + i] << (buff[1] - i - 1) * 8;
        // pridani jednotlivych oktetu integeru
        enc << buff[2 + i];
        len++;
    }
    
    return true;
}

long IntegerBER::value(void)
{
    return result;
}

std::string IntegerBER::encoded(void)
{
    return enc.str();
}

short IntegerBER::numOfOctets(void)
{
    return len;
}

/******************************************************************************/

void LengthBER::decode(const unsigned char *buff)
{
    // inicializace
    result = 0;
    len = 1;
    
    // celkova velikost se vlezla na jeden oktet
    if (buff[0] < 0x80)
    {   
        result = buff[0];
        return;
    }
    
    // jinak vypocitej celkovy pocet oktetu
    int octets = buff[0] - 0x80;
    len += octets;
    
    // vysledna hodnota
    for (int i = 0; i < octets; i++)
    {
        // podle celkoveho poctu oktetu se shiftuji jednotlive hodnoty o pozadovany pocet oktetu
        result += buff[1 + i] << (octets - i - 1) * 8;
    }
}

long LengthBER::value(void)
{
    return result;
}

short LengthBER::numOfOctets(void)
{
    return len;
}

/******************************************************************************/

void StrBER::encode(const char *str)
{
    // inicializace
    len = 2; // typ OCTET STRING + jeho delka LENGTH
    en_val.str(std::string());
    
    // delka vstupniho retezce
    int str_len = std::string(str).length();

    en_val << (unsigned char)(0x04);        // OCTET STRING
    en_val << (unsigned char)(str_len);     // LENGTH
    for (int c = 0; c < str_len; c++)       // VALUE
    {
        en_val << (unsigned char)(str[c]);
        len++;
    }
}

bool StrBER::decode(const unsigned char *buff, int msg_len)
{
    // inicializace
    len = 1; // jeden oktet pro 0x04
    de_val.str(std::string());
    // pomocny objekt na zpracovani delky
    LengthBER ber_len;
    
    // kontrola, zdali se jedna o typ OCTET STRING
    if (buff[0] != 0x04)
        return false;
    
    // ziskani delky stringu
    ber_len.decode(&buff[1]);
    long length = ber_len.value();
    len += ber_len.numOfOctets();
    
    // kontrola delky stringu - nemuze mit vice oktetu nez je poslano
    if (length > (msg_len - 2))
        return false;
    
    // ziskani hodnoty stringu
    for (int c = 0; c < length; c++)
    {    
        de_val << buff[ber_len.numOfOctets() + 1 + c];
        len++;
    }
    
    return true;
}

std::string StrBER::decodedValue(void)
{
    return de_val.str();
}


std::string StrBER::encodedValue(void)
{
    return en_val.str();
}

short StrBER::numOfOctets(void)
{
    return len;
}

/******************************************************************************/

void PartialAttribute::encode(const char *type, const char *val)
{
    // inicializace    
    StrBER t;
    StrBER v;
    t.encode(type);
    v.encode(val);
    en_val.str(std::string());
    
    // celkova delka je (SEQUENCE + LENGTH = 2 oktety) + delka type + (SET + LENGTH = 2 oktety) + delka value
    len = 2 + t.numOfOctets() + 2 + v.numOfOctets();
    
    en_val << (unsigned char)(0x30);            // SEQUENCE
    en_val << (unsigned char)(len - 2);         // LENGTH  (-2 kvuli prvnim dvoum oktetum, ktere se nezahrnuji)
    en_val << t.encodedValue();                 // kompletni OCTET STRING
    en_val << (unsigned char)(0x31);            // SET
    en_val << (unsigned char)(v.numOfOctets()); // LENGTH
    en_val << v.encodedValue();                 // kompletni OCTET STRING
}

std::string PartialAttribute::encodedValue(void)
{
    return en_val.str();
}

short PartialAttribute::numOfOctets(void)
{
    return len;
}

/******************************************************************************/

bool LDAPFilter::decode(const unsigned char *buff, int msg_len)
{
    // offset buffer
    short off = 0;
    
    // pomocny objekt na zpracovani OCTET STRINGu 
    StrBER tmp;
    
    // pomocny objekt na zpracovani delky
    LengthBER ber_len;
    
    
    // Kontrola typu filteru
    type = buff[off++];
    if (type < filter_and || type > filter_substr )
        return false;
    
    // Delka filtru
    ber_len.decode(&buff[off]);
    long fil_len = ber_len.value();
    
    // posunuti offsetu
    off += ber_len.numOfOctets();
    
    // Kontrola delky filteru
    if (fil_len == 0x00 || fil_len > (msg_len - 2))
        return false;
        
    switch(type)
    {
        case filter_and:
            // dokud je zde nejaky filtr - offset neni na konci nadrazeneho filtru
            while (off < fil_len)
            {
                // pomocny filtr
                LDAPFilter f_tmp;
                // Kontrola a zpracovani filtru
                if (!f_tmp.decode(&buff[off], (msg_len - off)))
                {
                    if (f_tmp.at_des == ad_unsupported)
                        at_des = ad_unsupported;
                        
                    return false;
                }
                // ulozeni filtru
                f_and.push_back(f_tmp);
                off += f_tmp.numOfOctets();
            }
            break;
        
        case filter_or:
            // dokud je zde nejaky filtr - offset neni na konci nadrazeneho filtru
            while (off < fil_len)
            {
                // pomocny filtr
                LDAPFilter f_tmp;
                // Kontrola a zpracovani filtru
                if (!f_tmp.decode(&buff[off], (msg_len - off)))
                {
                    if (f_tmp.at_des == ad_unsupported)
                        at_des = ad_unsupported;
                        
                    return false;
                }
                // ulozeni filtru
                f_or.push_back(f_tmp);
                off += f_tmp.numOfOctets();
            }
            break;
        
        case filter_not:
            // dokud je zde nejaky filtr - offset neni na konci nadrazeneho filtru
            while (off < fil_len)
            {
                // pomocny filtr
                LDAPFilter f_tmp;
                // Kontrola a zpracovani filtru
                if (!f_tmp.decode(&buff[off], (msg_len - off)))
                {
                    if (f_tmp.at_des == ad_unsupported)
                        at_des = ad_unsupported;
                        
                    return false;
                }
                // ulozeni filtru
                f_not.push_back(f_tmp);
                off += f_tmp.numOfOctets();
            }
            break;
        
        case filter_eq_match:
            // Kontrola a zpracovani AttributeDescription
            if (!tmp.decode(&buff[off], fil_len)) // "len - (off + 2)" je zbyvajici delka zpravy
                return false;
            
            // kontrola a ulozeni hodnoty
            if (!tmp.decodedValue().compare("uid") || !tmp.decodedValue().compare("UserID") )
                at_des = ad_uid;

            else if (!tmp.decodedValue().compare("cn") || !tmp.decodedValue().compare("CommonName") )
                at_des = ad_cn;

            else if (!tmp.decodedValue().compare("mail"))
                at_des = ad_mail;

            else
            {
                at_des = ad_unsupported;
                return false;
            }
                
            // posun v bufferu o pocet oktetu OCTET STRINGu
            off += tmp.numOfOctets();

             // Kontrola a zpracovani AssertionValue
            if(!tmp.decode(&buff[off], msg_len - (off + 2))) // "len - (off + 2)" je zbyvajici delka zpravy
                return false;
            
            // ulozeni hodnoty
            as_val = tmp.decodedValue();
                
            // posun v bufferu o pocet oktetu OCTET STRINGu
            off += tmp.numOfOctets();
            break;

        case filter_substr:
            // Kontrola a zpracovani AttributeDescription
            if (!tmp.decode(&buff[off], fil_len)) // "len - (off + 2)" je zbyvajici delka zpravy
                return false;
            
            // kontrola a ulozeni hodnoty
            if (!tmp.decodedValue().compare("uid") || !tmp.decodedValue().compare("UserID"))
                at_des = ad_uid;

            else if (!tmp.decodedValue().compare("cn") || !tmp.decodedValue().compare("CommonName") )
                at_des = ad_cn;

            else if (!tmp.decodedValue().compare("mail"))
                at_des = ad_mail;

            else
            {
                at_des = ad_unsupported;
                return false;
            }
             
            // posun v bufferu o pocet oktetu OCTET STRINGu
            off += tmp.numOfOctets();
            
            // Kontrola SEQUENCE
            if (buff[off++] != 0x30)
                return false;

            off++;
            // Kontrola a zpracovani substringu - dokud neprojdu cely filtr
            while (off < (fil_len + 1))
            {
                // kontrola delky substringu
                if (buff[off + 1] > (fil_len - off))
                    return false;

                // ulozeni jednotlivych pismen podretezce
                std::stringstream ss;
                for (int c = 0; c < buff[off + 1]; c++)
                {
                    ss << buff[off + 2 + c];
                }

                switch (buff[off])
                {
                    case 0x80: // initial
                        sub_init = ss.str();
                        break;
                        
                    case 0x81: // any
                        sub_any.push_back(ss.str());
                        break;
                        
                    case 0x82: // final
                        sub_fin = ss.str();
                        break;
                        
                    default: 
                        return false;
                }

                // pricteni delky substringu k offsetu
                off += (2 + buff[off + 1]);
            }
            
            break;
        
        // nemelo by nastat
        default:
            return false;
    }
    len = off;
    return true;
}

short LDAPFilter::numOfOctets(void)
{
    return len;
}

/******************************************************************************/

MyLDAPMsgDecoder::MyLDAPMsgDecoder(const unsigned char *buff, int len)
{
    clear();
    decode(buff, len);
}

void MyLDAPMsgDecoder::clear(void)
{
    // inicializace promennych
    rc   = rc_protocolError;
    type = -1;
}
    
void MyLDAPMsgDecoder::decode(const unsigned char *buff, int len)
{
    // inicializace
    clear();
    IntegerBER s_limit; // sizeLimit
    IntegerBER t_limit; // timeLimit
    // pomocny objekt pro zpracovani OCTET STRINGU
    StrBER tmp;
    
    // objekt pro zpracovani delek
    LengthBER ber_len;
    
    // offset v bufferu
    short off = 0;
    
    // LDAPMessage musi mit alespon 7 bytu (LDAPMsg Len MsgID Len Val protocolOp Len)
    if (len < 7)
        return;

    // Musi se jednat o LDAPMessage
    if (buff[off++] != 0x30)
        return;

    // musi mit spravnou delku
    ber_len.decode(&buff[off]);
    if (ber_len.value() > len - (off + ber_len.numOfOctets())) 
        return;
    
    // posun bufferu
    off += ber_len.numOfOctets();
    
    // Zpracovani a ulozeni MessageID 
    if (!id.decode(&buff[off], len - (off + 2))) // "len - (off + 2)" je zbyvajici delka zpravy
        return;
    
    // posun v bufferu o pocet oktetu integeru
    off += id.numOfOctets();

    // ulozeni typu
    type = buff[off++];
    
    // kontrola delky daneho typu
    ber_len.decode(&buff[off]);
    if (ber_len.value() > len - (off + ber_len.numOfOctets())) 
        return;

    // posun bufferu
    off += ber_len.numOfOctets();
    switch (type)
    {
        case ldap_bind:
            // version musi byt typu INTEGER o delce 1 v rozsahu 0...127, ale podpora je pouze pro verze 2-3
            if (buff[off++] != 0x02 || buff[off++] != 0x01 || buff[off] < 0x02 || buff[off++] > 0x03)
                return;

            // name musi byt typu OCTET STRING se spravnou delkou
            if (!tmp.decode(&buff[off], (len - off)))
                return;
                
            off += tmp.numOfOctets();
            
            // authentication simple - [0] OCTET STRING
            if (buff[off++] != 0x80)
            {
                rc = rc_authMethodNotSupported;
                return;
            }
            
            // nulova delka OCTET STRINGu
            if (buff[off] != 0x00)
                return;
                
            break;
        
        case ldap_search:
            // baseObject musi mit spravny format
            if (!tmp.decode(&buff[off], (len - off)))
                return;
                
            off += tmp.numOfOctets();
            
            // kontrola tvaru a delky parametru scope
            if (buff[off++] != 0x0a || buff[off++] != 0x01)
                return;

            // scope musi mit hodnotu 1, 2 nebo 3, ale vzdy se pracuje s base
            if (buff[off++] > 0x03)
                return;
            
            // kontrola tvaru, delky a hodnoty parametru derefAliases - musi mit hodnotu 1, 2 nebo 3, ale vzdy se pracuje s neverDerefAliases
            if (buff[off++] != 0x0a || buff[off++] != 0x01 || buff[off++] > 0x03)
                return;

            // Kontrola parametru sizeLimit
            if (!s_limit.decode(&buff[off], len - (off + 2))) // "len - (off + 2)" je zbyvajici delka zpravy
                return;

            // ulozeni hodnoty sizeLimit
            sl = s_limit.value();
            if (sl == 0)
                sl = 2147483647; // maxInt dle RFC 4511
                
            // posun v bufferu o pocet oktetu integeru
            off += s_limit.numOfOctets();

            // Kontrola parametru timeLimit
            if(!t_limit.decode(&buff[off], len - (off + 2))) // "len - (off + 2)" je zbyvajici delka zpravy
                return;
                
            // posun v bufferu o pocet oktetu integeru
            off += t_limit.numOfOctets();

            // Typ BOOLEAN, musi mit delku jedna
            if (buff[off++] != 0x01 || buff[off++] != 0x01)
                return;

            off++;

            // Kontrola a zpracovani filtru
            if (!filter.decode(&buff[off], len - off)) // "len - off" je zbyvajici delka zpravy
            {
                if (filter.at_des == ad_unsupported)
                    rc = rc_undefinedAttributeType;
                    
                return;
            }

            off += filter.numOfOctets();
            // Kontrola atributu
            if (buff[off++] != 0x30 || buff[off] > len - (off + 1))
                return;
                
            off++;
            // dokud neni offset na konci zpravy, ziskavej atributy
            while (off < len)
            {
                StrBER tmp;
                // zpracovani atributu
                if(!tmp.decode(&buff[off], len - off)) // "len - off" je zbyvajici delka zpravy vcetne octet stringu
                    return;
                
                // ulozeni atributu
                atts.push_back(tmp.decodedValue());
                
                // posun v bufferu o pocet oktetu OCTET STRINGu
                off += tmp.numOfOctets();
            }

            break;
        
        case ldap_unbind:
            if (buff[off++] != 0x00)
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

ulong MyLDAPMsgDecoder::sizeLimit(void)
{
    return sl;
}

IntegerBER *MyLDAPMsgDecoder::getID(void)
{
    return &id;
}

LDAPFilter *MyLDAPMsgDecoder::getFilter(void)
{
    return &filter;
}

std::vector<std::string> MyLDAPMsgDecoder::requiredAttributes(void)
{
    return atts;
}

/******************************************************************************/

std::string MyLDAPMsgConstructor::createBindResponse(IntegerBER *msg_id, int res_code)
{
    std::stringstream msg;
    
    msg << (unsigned char)(0x30);    // ---------- LDAPMessage
    msg << (unsigned char)(0x09 + msg_id->numOfOctets());
    msg << msg_id->encoded();        // ----- MessageID
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

std::string MyLDAPMsgConstructor::createSearchResultEntry(IntegerBER *msg_id, csv_record *rec, req_atts *req_att)
{
    // zpracovani objectName
    StrBER dn;
    std::string tmp("uid=");
    tmp.append(rec->uid);
    dn.encode(tmp.c_str()); // Distinguished Name je jednoznačný identifikátor záznamu v rámci databáze, což splňuje atribut uid (xlogin)
    
    // zpracovani PartialAttribute
    PartialAttribute cn, uid, mail;
    cn.encode("cn", rec->cn.c_str());
    uid.encode("uid", rec->uid.c_str());
    mail.encode("mail", rec->mail.c_str());
    
    // delka PartialAttributeList (podle klientem pozadovanych atributu)
    short p_len = 0;
    if (req_att->cn)
        p_len += cn.numOfOctets();
        
    if (req_att->uid)
        p_len += uid.numOfOctets();
        
    if (req_att->mail)
        p_len += mail.numOfOctets();
        
    std::stringstream msg;
    msg << (unsigned char)(0x30);           // ---------- LDAPMessage
    msg << (unsigned char)(msg_id->numOfOctets() + 0x04 + dn.numOfOctets() + p_len);
    msg << msg_id->encoded();               // ----- MessageID
    
    msg << (unsigned char)(0x64);           // ----- searchResEntry - [APPLICATION 5] SEQUENCE
    msg << (unsigned char)(0x02 + dn.numOfOctets() + p_len); //LENGTH
    msg << dn.encodedValue();         // ----- objectName
    msg << (unsigned char)(0x30);           // ----- PartialAttributeList - SEQUENCE
    msg << (unsigned char)(p_len);          // ----- LENGTH
    
    // pridani danych atributu dle pozadavku klienta
    if (req_att->cn)
        msg << cn.encodedValue();
    
    if (req_att->uid)
        msg << uid.encodedValue();
    
    if (req_att->mail)
        msg << mail.encodedValue();
    
    return msg.str();
}

std::string MyLDAPMsgConstructor::createSearchResultDone(IntegerBER *msg_id, int res_code)
{
    std::stringstream msg;
    
    msg << (unsigned char)(0x30);    // ---------- LDAPMessage
    msg << (unsigned char)(0x09 + msg_id->numOfOctets());
    msg << msg_id->encoded();        // ----- MessageID
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
