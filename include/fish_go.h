#pragma once
#include <vector>
#ifndef _FISH_GO_H_
#define _FISH_GO_H_

#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include <pqxx/pqxx>
//#include "db_go.h"

#define FISH_GO_DEBUG 0 // if > 0 turned on (partly)
#define FISH_GO_WARNING 0 //if > 0 turned on

#define PATH_TO_FISHES_DEFAULT "../generator_otchetov/fish/"
#define DB_NAME_FIELD "db_name"
#define DB_TABLE_NAME_FIELD "db_table_name"
#define REPLACEMENT_MARK "___"

//db:
//#define DB_NAME "otchet_f"
//#define DB_TABLE_NAME "physical_data"
#define DB_USER "postgres"
#define DB_HOSTADDR "127.0.0.1"
#define DB_PORT "5432"

#define COMMAND_DB_GET_ALL_FIELDS "SELECT column_name FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name ="

#define OTCHETY_FOLDER_NAME "otchety"

#define FILE_FORMAT ".txt"

using namespace std;

//����� Fish_go �������� � ����� � ����� ������, ������� �����
class Fish_go{
    vector<string> fields;// ���� �� ����
    vector<string> db_equivalent_fields; // ���� �� ���� ������
    string db_name;
    string db_table_name;
    string path_to_fish;
    string fish_without_db_and_table;
    pqxx::connection c; // connection to db
   public:
    class ExceptionFish_go { //����� ����������
      string message{};

     public:
      ExceptionFish_go() : message{"Error: Invalid Fish_go"} {} //�����������
      ExceptionFish_go(string s_in) : message{"Error: Invalid Fish_go: message: "+s_in} {} // ����������� � ����������
      string what() const { return message; } //����������� ������� �� �� �������� ������ ������
    };

    Fish_go(string path_in); //����������� ������� ���������� � ��
    //����� ������ �� ���� (db_name db_table_name fields)
    //����� ������������� ������ �� ���� ������
    //������� �����

    static void Debug_fish_go(string function_name_in, string message_in); //����������� ����� ������
    // ����� �������������� ����������

    void Warning_fish_go(string function_name_in,
                         string message_in); // ����� ��������������

    string InitDataReturnDbN(string path_in); // ������ ����
    //�������������� ���� db_name
    //�������������� ���� db_table_name
    //��������� ������ � ���������� ����� fields

    static string CreateDBconnStr (string db_name_in, string db_user_in,
                           string db_hostaddr_in, string db_port_in); //������� ������ ��� ������������� pqxx::connection

    const string & GetPath() const{return path_to_fish;}
    const vector <string> & GetFields() const{return fields;}
    void InitFields(vector<string> &fields_in, const string & fish_without_db_and_table); //�������������� ���� �� ����
    pqxx::connection* GetConnection() { return &c; }
    bool InitDbEqFields(); //�������������� ���� �� ���� ������, ������������� ��� fields (����� �� ����)
    string CreateEqRegex(const string db_field_in); // ������� ������ ��� ������ �������������� ���� � ���� ������
    pqxx::result Query(string command_in); //��������� ������ � ���� ������
    void CreateOtchet(const string &path_to_folder_in, const string& otchet_in, size_t number_of_row_in_db); //������� ���� ������
    void GenOtchety(); //������� ������ ��� ���� ����� ���� ������ ������� ���� � ���� ������� �� ������ ���� ������
    string ReturnNameOfFile(const string path_in); //�������� ��� ����� �� ����
//    ~Fish_go();
};

void CreateTablePhysicalData(); // ������� ������ ������� physical_data � ���� ������ otchet_f
#endif //!_FISH_GO_H_
