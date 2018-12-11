#pragma once

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

#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>


/**
 * Struktura reprezentujici pozadovane atributy
 */
struct req_atts
{
    // konstruktor
    req_atts(std::vector<std::string> atts);
    
    bool valid;  // Validita zaznamu
    bool cn;     // flag, zdali se pozaduje CN
    bool uid;    // flag, zdali se pozaduje UID
    bool mail;   // flag, zdali se pozaduje MAIL
};


/**
 * Struktura predstavujici jeden zaznam v csv souboru
 */
struct csv_record
{
    // konstruktor
    csv_record(std::string str);
    
    // upravi dane retezce - odstrani uvodni nuly kazdeho znaku
    std::string customize(std::string str);
    
    bool valid;         // Validita zaznamu
    std::string cn;     // CN
    std::string uid;    // UID
    std::string mail;   // MAIL
    
    friend bool operator== ( const csv_record &csv1, const csv_record &csv2 );
};

// pomocna funkce pro porovnavani dvou csv_record struktur
inline bool operator== ( const csv_record &csv1, const csv_record &csv2 )
{
    return ( ( csv1.cn == csv2.cn ) && ( csv1.uid == csv2.uid ) && ( csv1.mail == csv2.mail ) );
}

/******************************************************************************/

// Vycet podporovanych zprav od klienta
enum ProtocolOp : int
{
    ldap_bind   = 0x60,
    ldap_search = 0x63,
    ldap_unbind = 0x42
};

/******************************************************************************/

// Vycet navratovych kodu
enum ResultCode : int
{
    rc_success = 0x00,
    rc_protocolError = 0x02,
    rc_sizeLimitExceeded = 0x04,
    rc_authMethodNotSupported = 0x07,
    rc_noSuchAttribute = 0x10,
    rc_undefinedAttributeType = 0x11
    
};

/******************************************************************************/

// Vycet podporovanych filteru
enum Filters : int
{
    filter_and      = 0xa0,
    filter_or       = 0xa1,
    filter_not      = 0xa2,
    filter_eq_match = 0xa3,
    filter_substr   = 0xa4
};

/******************************************************************************/

// Vycet podporovanych atributu pro vyhledavani - AttributeDescription
enum AttributeDes : int
{
    ad_unsupported,
    ad_uid,
    ad_cn,
    ad_mail
};

/******************************************************************************/

/**
 *  Trida pro zpracovani BER integeru.
 */
class IntegerBER
{
    // hodnota integeru
    long result;
    
    // zakodovana zprava
    std::stringstream enc;
    
    // celkova delka integeru v oktetech
    short len;

public:

    // Zkontroluje spravnost integeru a ziska jeho hodnotu.
    // Vrati hodnotu true v pripade platneho integeru, jinak false.
    bool decode(const unsigned char *buff, int msg_len);
    
    // Vrati hodnotu daneho integeru.
    long value(void);
    
    // Vrati zakodovany integer - pro odeslani.
    std::string encoded(void);
    
    // Vrati delku integeru v oktetech.
    short numOfOctets(void);
};

/******************************************************************************/

/**
 *  Trida pro zpracovani delky zakodovane pomoci BER.
 */
class LengthBER
{
    // hodnota delky
    long result;
    
    // celkova delka LENGTH v oktetech
    short len;

public:

    // Vrati hodnotu true v pripade platne delky, jinak false.
    void decode(const unsigned char *buff);
    
    // Vrati hodnotu dane delky.
    long value(void);
    
    // Vrati celkovou  delku v oktetech.
    short numOfOctets(void);
};

/******************************************************************************/

/**
 * Trida pro zpracovani BER stringu - OCTET STRING.
 * Slouzi pro dekodovani nebo zakodovani urciteho retezce. 
 */
class StrBER
{
    // hodnota dekodovaneho stringu
    std::stringstream de_val;
    
    // hodnota zakodovaneho stringu
    std::stringstream en_val;
    
    // celkova delka stringu v oktetech
    short len;

public:

    // Zakoduje zadany retezec do BER OCTET STRING
    void encode(const char *str);

    // Zkontroluje spravnost stringu a ziska jeho hodnotu.
    // Vrati hodnotu true v pripade platneho stringu, jinak false.
    bool decode(const unsigned char *buff, int msg_len);
    
    // Vrati dekodovanou hodnotu daneho retezce.
    std::string decodedValue(void);
    
    // Vrati zakodovanou hodnotu daneho retezce.
    std::string encodedValue(void);
    
    // Vrati delku stringu v oktetech.
    short numOfOctets(void);
};

/******************************************************************************/

/**
 *  Trida pro reprezentujici prvek PartialAttribute
 */
class PartialAttribute
{
    // hodnota zakodovaneho stringu
    std::stringstream en_val;
    
    // celkova delka stringu v oktetech
    short len;
    
public:
    
    // Zakoduje zadany typ a jeho prislusnou hodnotu
    void encode(const char *type, const char *val);
    
    // Vrati zakodovanou hodnotu daneho retezce.
    std::string encodedValue(void);
    
    // Vrati delku stringu v oktetech.
    short numOfOctets(void);
};

/******************************************************************************/

/**
 *  Trida pro dekodovani a zpracovani LDAP filtru.
 */
class LDAPFilter
{ 
public:

    int type;   // typ filtru
    short len;  // velikost filtru v oktetech
    int at_des; // AttributeDescription
    
    std::string as_val;                 // AssertionValue
    std::string sub_init;               // initial substring
    std::vector<std::string> sub_any;   // vektor substringu pro any
    std::string sub_fin;                // final substring
    
    std::vector<LDAPFilter> f_and;  // jednotlive filtry pro AND
    std::vector<LDAPFilter> f_or;   // jednotlive filtry pro OR
    std::vector<LDAPFilter> f_not;  // jednotlive filtry pro NOT
    
    // Zkontroluje spravnost filtru a ziska jeho atributy.
    // Vrati hodnotu true v pripade platneho a podporovaneho filteru, jinak false.
    bool decode(const unsigned char *buff, int msg_len);
    
    // vrati velikost filtru
    short numOfOctets(void);
};
 
/******************************************************************************/

/**
 *  Trida pro dekodovani LDAP zprav.
 */
class MyLDAPMsgDecoder
{
    // Navratova hodnota zpravy - result code
    int rc;
    
    // id zpravy
    IntegerBER id;
    
    // vychozi filter u searchReq
    LDAPFilter filter;
    
    // typ zpravy
    int type;
    
    // sizeLimit
    ulong sl;
    
    // pozadovane atributy
    std::vector<std::string> atts;
    
public:

    // Dekoduje obsah bufferu
    MyLDAPMsgDecoder(const unsigned char *buff, int len);
    
    // Reinicializuje objekt
    void clear(void);
    
    // Dekoduje obsah bufferu
    void decode(const unsigned char *buff, int len);
    
    // Vrati resultCode (hodnota promenne rc)
	int resultCode(void);
    
    // Vrati typ dane zpravy
    int getType(void);
    
    // Vrati hodnotu maximalniho poctu searchEntry zprav
    ulong sizeLimit(void);
    
    // Vrati ID zpravy
    IntegerBER *getID(void);
    
    // Vrati filter
    LDAPFilter *getFilter(void);
    
    // vrati pozadovane atributy
    std::vector<std::string> requiredAttributes(void);
};

/******************************************************************************/

/**
 *  Trida pro vytvareni LDAP zprav.
 */
class MyLDAPMsgConstructor
{
public:

    // Vytvori zpravu bindResponse s danym MessageID a resultCode
    static std::string createBindResponse(IntegerBER *msg_id, int res_code);
    
    // Vytvori zpravu SearchResultEntry s danym MessageID
    static std::string createSearchResultEntry(IntegerBER *msg_id, csv_record *rec, req_atts *req_att);
    
    // Vytvori zpravu searchResultDone s danym MessageID a resultCode
    static std::string createSearchResultDone(IntegerBER *msg_id, int res_code);
};
