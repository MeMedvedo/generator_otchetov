#include "fish_go.h"

Fish_go::Fish_go(string path_in)
    : path_to_fish{path_in},
      c{CreateDBconnStr(InitDataReturnDbN(path_in), DB_USER, DB_HOSTADDR,
                        DB_PORT)} {
#if FISH_GO_DEBUG == 1
  Debug_fish_go("Fish_go", string("") + "c.is_open() = " +
                               (c.is_open() == 0 ? "false" : "true") + "\n" +
                               "c.dbname() = " + c.dbname());
#endif
  if (InitDbEqFields()) {
#if FISH_GO_DEBUG == 1
    Debug_fish_go("Fish_go", "ready for GenOtchety");
#endif
#if FISH_GO_DEBUG >= 1
    for (size_t i = 0; i < fields.size(); ++i) {
      Debug_fish_go("Fish_go", string("fields[") + to_string(i) +
                                   "] = " + fields[i] + "\n" +
                                   "db_equivalent_fields[" + to_string(i) +
                                   "] = " + db_equivalent_fields[i]);
    }
#endif
    GenOtchety();
  }
}

string Fish_go::CreateDBconnStr(string db_name_in, string db_user_in,
                                string db_hostaddr_in, string db_port_in) {
  string ready_str{};
  ready_str = ready_str + "dbname = " + db_name_in + " user = " + db_user_in +
              " hostaddr = " + db_hostaddr_in + " port = " + db_port_in;

#if FISH_GO_DEBUG == 1
  Debug_fish_go("CreateDBconnStr", std::string{"ready_str: " + ready_str});
#endif

  return ready_str;
}

void Fish_go::Debug_fish_go(string function_name_in, string message_in) {
  string str = "!!!DEBUG Fish_go, function:" + function_name_in +
               " message: " + message_in;
  cout << str << endl;
}

string Fish_go::InitDataReturnDbN(string path_in) {
  ifstream ifs;
  ifs.open(path_in);
  if (!ifs.is_open()) {
    throw Fish_go::ExceptionFish_go("Fish_go::Fish_go: can't open fish file");
  }

  cmatch result;

  //  regex regular(string(DB_NAME_FIELD) +
  //                "\\s*(\\w*)\\s" +  // get value of db_name field
  //                string(DB_TABLE_NAME_FIELD) +
  //                "\\s*(\\w*)\\s*"   // get value of db_table_name field
  //                "((\\w*\\s*)*)");  // get values of fields

  regex regular(string(DB_NAME_FIELD) +
                "\\s*(\\w*)\\s" +  // get value of db_name field
                string(DB_TABLE_NAME_FIELD) +
                "\\s*(\\w*)\\s*"  // get value of db_table_name field
                "([\\s\\S]*)");   // get values of fields

  string f_str;
  ostringstream ss;
  ss << ifs.rdbuf();  // reading data
  f_str = ss.str();

  if (std::regex_search(f_str.c_str(), result, regular)) {
    if (result[1] == "") {
      throw Fish_go::ExceptionFish_go(
          "Fish_go::Init_data: there is no db_name in fish");
    }
    db_name = result[1];  // 1 means second value

    if (result[2] == "") {
      throw Fish_go::ExceptionFish_go(
          "Fish_go::Init_data: there is no db_table_name in fish");
    }
    db_table_name = result[2];  // 2 means third value

    if (result[3] == "") {
      throw Fish_go::ExceptionFish_go(
          "Fish_go::Init_data: there is no fields in fish");
    }
    f_str = result[3];  // f_str = values of fields
  }

  fish_without_db_and_table = f_str;

#if FISH_GO_DEBUG == 1
  Debug_fish_go("InitDataReturnDbN", string("fish without db and table = ") +
                                         fish_without_db_and_table);
#endif
  // init fields
  InitFields(fields, f_str);

#if FISH_GO_DEBUG == 1
  Debug_fish_go("InitDataReturnDbN", string("db_name = ") + db_name);
  Debug_fish_go("InitDataReturnDbN",
                string("db_table_name = ") + db_table_name);
  Debug_fish_go("InitDataReturnDbN", string("f_str (with fields) = ") + f_str);
  Debug_fish_go("InitDataReturnDbN",
                string("fields.size() = ") + to_string(fields.size()));
  for (size_t i = 0; i < fields.size(); ++i) {
    Debug_fish_go("InitDataReturnDbN",
                  string("field[" + std::to_string(i) + "] = ") + fields[i]);
  }
#endif
  ifs.close();

  return db_name;
}

void Fish_go::InitFields(vector<string> &fields_in,
                         const string &fish_without_db_and_table) {
  cmatch result;

  regex regular(string("(") + REPLACEMENT_MARK +
                "(([a-zA-Z0-9]+[_ ]?)*[a-zA-Z0-9])" + REPLACEMENT_MARK +
                ")([\\s\\S]*)");

  string field{};

  bool success_search =
      std::regex_search(fish_without_db_and_table.c_str(), result, regular);

  string part = fish_without_db_and_table;

  while (success_search) {
    fields_in.emplace_back(result[2]);
    part = result[4];
    success_search = std::regex_search(part.c_str(), result, regular);
  }

  if (!fields_in.size()) {
#if FISH_GO_WARNING == 1
    Warning_fish_go(
        "InitFields",
        string("no fields have been initialized in ") + path_to_fish);
#endif
  }
}

// Fish_go::~Fish_go() {
//   //  Debug_fish_go("~Fish_go", "Destructor");
//   // https://libpqxx.readthedocs.io/en/6.4/a00867.html
//   // The connection is automatically closed upon destruction (if it hasn't
//   been
//   // closed already)
// }

bool Fish_go::InitDbEqFields() {
  if (!fields.size()) {
#if FISH_GO_WARNING == 1
    Warning_fish_go("InitDbEqFields",
                    string("fields.size() == 0, otchety for ") + path_to_fish +
                        " will not be made");
#endif

    return false;
  }

  // get all db fields
  pqxx::work w(c);
  pqxx::result all_db_fields =
      w.exec(string(COMMAND_DB_GET_ALL_FIELDS) + "'" + db_table_name + "'");

  for (size_t i = 0; i < fields.size(); ++i) {
#if FISH_GO_DEBUG >= 1
    Debug_fish_go("InitDbEqFields",
                  string("fields[") + to_string(i) + "] = " + fields[i]);
#endif
    cmatch result;
    regex regular(CreateEqRegex(fields[i]));

    for (int k = 0; k < all_db_fields.size(); ++k) {
      if (std::regex_search(all_db_fields[k][0].c_str(), result, regular)) {
        db_equivalent_fields.emplace_back(result[0]);
        break;  // go out of the internal cycle
      }
    }
    if ((i + 1) != db_equivalent_fields.size()) {
#if FISH_GO_WARNING == 1
      Warning_fish_go("InitDbEqFields",
                      string("db_equivalent_fields.size() !=  fields.size(), "
                             "otchety for") +
                          path_to_fish + " will not be made");
#endif
      return false;
    }
  }

  //  bool equal{db_equivalent_fields.size() == fields.size()};

  //  if (!equal) {
  //    cout << "!!!WARNING db_equivalent_fields.size() !=  fields.size(),
  //    otchety "
  //            "for"
  //         << path_to_fish << " will not be made" << endl;
  //  }
  //
  //  return equal;
  return true;
}

string Fish_go::CreateEqRegex(const string db_field_in) {
  string reg{};
  for (size_t i = 0; i < db_field_in.size(); ++i) {
    char ch = db_field_in[i];

    reg = reg + "[";
    reg = reg + ch;

    if (isalpha(ch)) {
      if (isupper(ch)) {
        reg = reg + static_cast<char>(tolower(ch));
      } else {  // if (islower(ch))
        reg = reg + static_cast<char>(toupper(ch));
      }
    } else if (ch == '_') {
      reg = reg + " ";
    } else if (ch == ' ') {
      reg = reg + "_";
    } else if (isdigit(ch)) {
      // do nothing
    } else {
      throw Fish_go::ExceptionFish_go(string("in CreateEqRegex: db_field_in[") +
                                      std::to_string(i) +
                                      "] is not alpha or _ or space or digit!");
    }

    reg = reg + "]";
  }
#if FISH_GO_DEBUG == 1
  Debug_fish_go("CreateEqRegex", string("regex = ") + reg);
#endif

  return reg;
}

void Fish_go::GenOtchety() {
  // check existence of otchety folder
  auto catalogue_of_fishes =
      filesystem::directory_iterator(PATH_TO_FISHES_DEFAULT);
  bool exists = false;
  for (const auto &file : catalogue_of_fishes) {
    if (file.is_directory() && file.path().filename() == OTCHETY_FOLDER_NAME) {
      exists = true;
      break;
    }
  }

#if FISH_GO_DEBUG >= 1
  Debug_fish_go("GenOtchety",
                string("folder ") + OTCHETY_FOLDER_NAME +
                    " exists = " + (exists == 0 ? "false" : "true"));
#endif

  string path_to_otchety{string(PATH_TO_FISHES_DEFAULT) + "/" +
                         OTCHETY_FOLDER_NAME};

  if (!exists) {
    filesystem::create_directory(path_to_otchety);
  }

  bool data_not_empty = true;
  size_t db_row = 0;
  for (; data_not_empty; ++db_row) {
    string otchet{fish_without_db_and_table};
    for (size_t j = 0; j < fields.size(); ++j) {
      regex regular(string(REPLACEMENT_MARK) + fields[j] + REPLACEMENT_MARK);
      string q = "SELECT " + db_equivalent_fields[j] + " from " +
                 db_table_name + " LIMIT 1 " + "OFFSET " + to_string(db_row);

#if FISH_GO_DEBUG >= 1
      Debug_fish_go("GenOtchety", string("q = ") + q);
#endif

      auto data = Query(q);

      if (data.empty()) {
        data_not_empty = false;
        break;
      } else {
#if FISH_GO_DEBUG >= 1
        Debug_fish_go("GenOtchety",
                      string("data[0][0] = ") + data[0][0].c_str());
#endif

        otchet = regex_replace(otchet.c_str(), regular, data[0][0].c_str());
      }
    }
    if (data_not_empty) {
#if FISH_GO_DEBUG >= 1
      Debug_fish_go("GenOtchety",
                    string("Otchet ") + to_string(db_row) + ": \n" + otchet);
#endif

      string path_to_otchety_of_fish_folder =
          path_to_otchety + "/" + ReturnNameOfFile(path_to_fish);
      filesystem::create_directory(path_to_otchety_of_fish_folder);
      CreateOtchet(path_to_otchety_of_fish_folder, otchet, db_row);
    }
  }
}

pqxx::result Fish_go::Query(string command_in) {
  pqxx::work txn{c};
  return txn.exec(command_in);
}

void Fish_go::CreateOtchet(const string &path_to_folder_in,
                           const string &otchet_in,
                           size_t number_of_row_in_db) {
  ofstream fout;
  string full_path_to_file{path_to_folder_in + "/" +
                           to_string(number_of_row_in_db) + FILE_FORMAT};
  fout.open(full_path_to_file);  // create\rewrite file

  if (!fout.is_open()) {
    throw Fish_go::ExceptionFish_go("can't open file " + full_path_to_file);
  } else {
    fout << otchet_in;
  }
  fout.close();
}

string Fish_go::ReturnNameOfFile(const string path_in) {
  string answer{};

  bool add_to_ans = false;

  for (size_t i = path_in.size() - 1; i >= 0; --i) {
    if (add_to_ans) {
      answer.push_back(path_in[i]);  // reverse adding
    }

    if (path_in[i] == '.') {
      add_to_ans = true;
    }

    if (path_in[i] == '/') {
      // add_to_ans = false;
      break;
    }
  }
  answer = string(answer.rbegin(), answer.rend());  // reverse answer
#if FISH_GO_DEBUG >= 1
  Debug_fish_go("ReturnNameOfFile", string("name of file = ") + answer);
#endif
  return answer;
}

void Fish_go::Warning_fish_go(string function_name_in, string message_in) {
  string str = "!!!WARNING Fish_go, function:" + function_name_in +
               " message: " + message_in;
  cout << str << endl;
}

void CreateTablePhysicalData() {
  pqxx::connection c{

      Fish_go::CreateDBconnStr("otchet_f", DB_USER, DB_HOSTADDR, DB_PORT)};
  pqxx::work txn{c};
  txn.exec("drop table if exists physical_data");
  txn.exec(
      "CREATE TABLE physical_data (First_name VARCHAR(20), Last_name "
      "VARCHAR(20), Age INT, Gender VARCHAR(1), Growth real)");

  txn.exec(
      "INSERT INTO physical_data (first_name, last_name, age, gender, growth) "
      "VALUES ('Vasiliy', 'Petrov', 81, 'm', 169)");
  txn.exec(
      "INSERT INTO physical_data (first_name, last_name, age, gender, growth) "
      "VALUES ('Ivan', 'Ivanov', 24, 'm', 173)");
  txn.exec(
      "INSERT INTO physical_data (first_name, last_name, age, gender, growth) "
      "VALUES ('Mariya', 'Antonova', 31, 'f', 159)");
  txn.exec(
      "INSERT INTO physical_data (first_name, last_name, age, gender, growth) "
      "VALUES ('Vasilisa', 'Prekrasnaya', 29, 'f', 170)");
  txn.exec(
      "INSERT INTO physical_data (first_name, last_name, age, gender, growth) "
      "VALUES ('Vladislav', 'Tochkov', 41, 'm', 205)");
  txn.exec(
      "INSERT INTO physical_data (first_name, last_name, age, gender, growth) "
      "VALUES ('Vladimir', 'Vladimirov', 69, 'm', 170)");
  txn.exec(
      "INSERT INTO physical_data (first_name, last_name, age, gender, growth) "
      "VALUES ('Alina', 'Kurochkina', 81, 'f', 159)");
  txn.exec(
      "INSERT INTO physical_data (first_name, last_name, age, gender, growth) "
      "VALUES ('Svetlana', 'Kurochkina', 31, 'f', 172)");
  txn.exec(
      "INSERT INTO physical_data (first_name, last_name, age, gender, growth) "
      "VALUES ('Anastasiya', 'Berns', 45, 'f', 173)");
  txn.exec(
      "INSERT INTO physical_data (first_name, last_name, age, gender, growth) "
      "VALUES ('Anatoliy', 'Vlasov', 69, 'm', 185)");

  txn.commit();
}
