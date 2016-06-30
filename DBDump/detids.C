void detids(){

  // input
  std::ifstream input;
  input.open("ecalpeds/dump_EcalPedestals__since_00254286_till_00254636.dat",std::ios::in);
  float x, y, z, m1, r1, m2, r2, m3, r3;
  int id;
  
  // output
  std::ofstream output;
  output.open("detids.txt",std::ios::app);

  while (input >> x >> y >> z >> m1 >> r1 >> m2 >> r2 >> m3 >> r3 >> id){
    output << id << std::endl;
  }
  input.close();
  output.close();



}
