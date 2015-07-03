#ifndef __QUANTILE
#define __QUANTILE

#include <vector>
#include <limits>
#include <algorithm>

template<class T>
class Quantile{

  struct Data{
    Data(double v, const T& i): value(v), info(i) {}

    double value;
    T      info;

    bool operator<(const Data& a) const{
      return this->value < a.value;
    }
  };
  
  std::vector<Data> data;
  bool sorted;

 public:
  Quantile(): sorted(true){}
    void fill(double val, const T& info){
      data.push_back(Data(val, info));
      sorted = false;
    }

    void reset(){ data.clear(); }

    double xlow(double prop){
      if(prop < 0 || prop > 1)  return -std::numeric_limits<double>::max();
      sort();
      double i = data.size() * prop;
      if(data.size()==0) return -std::numeric_limits<double>::max();
      if(i==0) return data[0].value;
      if(i>=data.size()) return data[data.size()-1].value;
      return 0.5 * (data[i-1].value + data[i].value);
    }

    void sort(){
      if(!sorted) std::sort(data.begin(), data.end());
      sorted = true;
    }

    double xhigh(double prop){
      return xlow(1-prop);
    }
  

    std::vector<Data> tail_low(double prop){
      if(prop < 0 || prop > 1)  return -std::numeric_limits<double>::max();
      sort();
      double i = data.size() * prop;
      if(i > data.size()) i = 0;
      std::vector<Data> v(i);
    
      std::copy(data.begin(), data.begin() + i, v.begin());
    
      return v;
    }

    std::vector<Data> tail_high(double prop){
      if(prop < 0 || prop > 1)  return -std::numeric_limits<double>::max();
      sort();
      double i = data.size() * (1-prop);
      if(i > data.size()) i = 0;
      std::vector<Data> v(data.size() - i);
  
      for(int j = 0; j < v.size(); ++j){
	v[j] = data[data.size()-j];
      }
      return v;
    }
};

#endif
