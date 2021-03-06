#ifndef _NEURAL_NETWORK_H__
#define _NEURAL_NETWORK_H__

#include <iostream>
#include <vector>
#include "weights.h"
#include "Layer.h"
#include <string>
#include <list>

struct LayerEdge;
struct LayerNode;
class nn_iterator;

struct LayerEdge //Struct for edges in graph.
{
  struct LayerNode* l; //Pointer to the LayerNode endpoint of the edge.

  LayerEdge()
  {
    l=NULL;
  }
};


struct LayerNode //Struct for each node in the graph representing a layer in the nerual network.
{
  Layer* layer; //Pointer to layer object which contains all the metadata.
  std::vector<LayerEdge> edges; //Output edges of the node.
  std::vector<LayerNode*> inputs; //Input edges of the node.
  bool visited;

  LayerNode()
  {
    layer=NULL;
    visited=false;
  }
  LayerNode& operator=(const LayerNode& a){
      // Copying edges:
      std::copy(a.edges.begin(), a.edges.end(), back_inserter(edges));
      visited= a.visited;
      layer = a.layer;
      return *this;
  }
};

class NeuralNetwork
{
  public:
    typedef nn_iterator iterator;
    friend class nn_iterator;
  public:
    std::vector<LayerNode*> layers; //All layers in graph
    std::vector<LayerNode*> input; //Input layers for graph.

    LayerNode* findNode(std::string ID); //Finds node in graph

  public:
    NeuralNetwork(); //Constructor
    ~NeuralNetwork(); //Destructor

    void addLayer(Layer* layer); //Adds a layer
    void addEdge(Layer* start, Layer* end); //Adds an edge from start to end.
    void reset(); //Resets indices and visited boolean

    iterator begin();
    iterator end();
};

class nn_iterator
{
  public:
    typedef LayerNode value_type;
    typedef LayerNode& reference;
    typedef LayerNode* pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef void difference_type; //Doesnt really make sense in this case to have anything else.

    nn_iterator(NeuralNetwork* nn_, LayerNode* node_) : nn(nn_), node(node_) {nodeList.push_back(node);}
    nn_iterator() {}
    ~nn_iterator() {}
    nn_iterator& operator++(); //Prefix, increment return.
    nn_iterator operator++(int dummy); //Postfix, return increment.
    reference operator*() {return *node;} //Needs to be diferent
    pointer operator->() {return node;} //Needs to be diferent maybe?
    nn_iterator& operator=(const nn_iterator &rhs) {nn = rhs.nn; node = rhs.node; nodeList = rhs.nodeList; return *this;}
    bool operator==(const nn_iterator &rhs) {return (nn == rhs.nn && node == rhs.node);}
    bool operator!=(const nn_iterator &rhs) {return (nn != rhs.nn || node != rhs.node);}

    //Functions for code generation
    //NOTE: Not using these currently
    Weight* getWeight();
    Weight* getBias();
    void getInputs();
    Layer* getLayer();

  private:
    NeuralNetwork* nn;
    LayerNode* node; //Start
    std::list<LayerNode*> nodeList; //Node queue
};

#endif
