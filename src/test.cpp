#include "test.h"

void Test() {
  string path_to_fishes{PATH_TO_FISHES_DEFAULT};
  Fish_go fish1{(filesystem::directory_iterator(path_to_fishes))->path()};
  cout << fish1.GetConnection()->username() << endl;
  pqxx::work w(*fish1.GetConnection());
  pqxx::result r = w.exec(
      "SELECT column_name FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name ="
      "'physical_data'");
  cout << r[0][0].c_str() << std::endl;
  for (int i = 0; i < r.size(); ++i) {
    cout << "i = " << i << endl;
    //      for (int k = 0; k < r[i].size(); ++k) {
    //      cout << "k = " << k << endl;
    //        cout << r[i][k].c_str() << endl;
    cout << r[i][0].c_str() << endl;
    //      }
  }
  cout << r.size() << endl;

  TestLight();
}

void TestLight() {
  string path_to_fishes{PATH_TO_FISHES_DEFAULT};

  auto catalogue_of_fishes = filesystem::directory_iterator(path_to_fishes);
  string current_path = std::filesystem::current_path();
  cout << "current_path = " << current_path << endl;

  vector<Fish_go*> vfg;
  for (const auto& file : catalogue_of_fishes) {
    if (!file.is_directory()) {  // files only, no directories
      cout << endl << file.path().filename() << endl;
      Fish_go* temp_fg_ptr =
          new Fish_go(file.path().c_str());  //выделение памяти
      vfg.emplace_back(temp_fg_ptr);
    }
  }

  for (size_t i = 0; i < vfg.size(); ++i) {  //очистка памяти
    delete vfg[i];
  }
  vfg.clear();  //удаление элементов (то есть указателей на Fish_go)
}
