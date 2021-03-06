#include <iostream>
#include <vector>
#include <cstdlib>

#include "datastructures/tensor.h"
#include "generator/weight_generator.h"
#include "datastructures/weights.h"
#include "generator/layer_generator.h"
#include "generator/code_generator.h"

#include "datastructures/Layer.h"
#include "datastructures/NeuralNetwork.h"

NeuralNetwork* makeNet()
{
//	Weight* w1 = new Weight("conv1_weight", std::vector<unsigned int>({2,3,4}));
//  Weight* w2 = new Weight("conv1_bias", std::vector<unsigned int>({4}));

    std::vector<unsigned int> vec1;
    std::vector<unsigned int> vec2;

    vec1.push_back(2);
    vec1.push_back(3);
    vec1.push_back(4);
    vec2.push_back(4);

    Weight* w1 = new Weight("conv1_weight", vec1);
    Weight* w2 = new Weight("conv1_bias", vec2);

    Tensor<double>* v1 = w1->get_weight_tensor();
	Tensor<double>* v2 = w2->get_weight_tensor();

	// Populate with random doubles
	for(int i=0; i<2; i++)
	{
		for(int j=0; j<3; j++)
		{
			for(int k=0; k<4; k++)
			{
				(*v1)(i,j,k) = (double)rand() / RAND_MAX;
			}
		}
	}

	for(int i=0; i<4; i++)
	{

		(*v2)(i) = (double)rand() / RAND_MAX;
	}

  Weight* w3 = new Weight("dense_weight", vec1);
  Weight* w4 = new Weight("dense_bias", vec2);

  Tensor<double>* v3 = w3->get_weight_tensor();
  Tensor<double>* v4 = w4->get_weight_tensor();

  // Populate with random doubles
  for(int i=0; i<2; i++)
  {
    for(int j=0; j<3; j++)
    {
      for(int k=0; k<4; k++)
      {
        (*v3)(i,j,k) = (double)rand() / RAND_MAX;
      }
    }
  }

  for(int i=0; i<4; i++)
  {

    (*v4)(i) = (double)rand() / RAND_MAX;
  }

	NeuralNetwork* nn = new NeuralNetwork();

	InputLayer* input_layer = new InputLayer("layer0");
	Conv1D* conv1d_layer = new Conv1D("layer1");
	Dense* dense_layer = new Dense("layer2");

  conv1d_layer->w = w1;
  conv1d_layer->b = w2;
  conv1d_layer->filters = 1;
  conv1d_layer->kernel_size.push_back(5);
  conv1d_layer->strides = 3;
  conv1d_layer->input_shape.push_back(1);
  conv1d_layer->input_shape.push_back(2);
  dense_layer->w = w3;
  dense_layer->b = w4;
  dense_layer->input_shape.push_back(1);
  dense_layer->input_shape.push_back(2);
  dense_layer->activation="linear";
  dense_layer->units = 5;

  input_layer->layer_type = "input";

	nn->addLayer(input_layer);
	nn->addLayer(conv1d_layer);
	nn->addLayer(dense_layer);
	nn->addEdge(input_layer, conv1d_layer);
	nn->addEdge(conv1d_layer, dense_layer);

	return nn;
}

int main(int argc, char** argv)
{
	std::cout << "Making the Neural Network" << std::endl;
	NeuralNetwork* NN = makeNet();

  NeuralNetwork::iterator it;

  std::cout << "  Neural Network: " << std::endl;

  for(it = NN->begin(); it != NN->end(); it++)
  {
    std::cout << it->layer->layer_type << std::endl;
  }

    NN->reset();

    std::cout << "Making the Code Generator object" << std::endl;
	CodeGenerator* code_gen = new CodeGenerator(NN, "../templates/c_standard", "../example_out");
	std::cout << "Generating Layer and Weight stuff" << std::endl;
	code_gen->generate();
	std::cout << "Dumping the code" << std::endl;
	code_gen->dump();

  // delete w1;
  // delete w2;
  // delete w3;
  // delete w4;
  // delete input_layer;
  // delete conv1d_layer;
  // delete dense_layer;
  delete NN;

}
