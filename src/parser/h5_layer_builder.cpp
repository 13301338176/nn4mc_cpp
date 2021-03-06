#include "parser/h5_layer_builder.h"
#include <string>

void DropoutBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){
    json object = obj["config"];
    this->layerObject->identifier.assign(id);
    layerMap[this->layerObject->identifier] = this->layerObject;
    std::cout<< "LAYER_BUILDER: Dropout layer "<< this->layerObject->identifier << " recognied"<<std::endl;
}

void ActivationBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){
    json object = obj["config"];
    this->layerObject->identifier.assign(id);
    this->layerObject->activation.assign(object["activation"].get<std::string>());
    layerMap[this->layerObject->identifier] = this->layerObject;
    std::cout<< "LAYER_BUILDER: Activation layer "<< this->layerObject->identifier << " recognized"<<std::endl;
}

void Conv1DBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){
    json object= obj["config"];
    
    if (object["batch_input_shape"].size() > 0){ // this layer is the input layer
        this->layerObject->input_shape.push_back(object["batch_input_shape"][1]);
        this->layerObject->input_shape.push_back(object["batch_input_shape"][2]);
    } 
    this->layerObject->identifier.assign(id);
    this->layerObject->filters= object["filters"];
    this->layerObject->kernel_size.push_back(object["kernel_size"][0]);
    this->layerObject->strides = object["strides"][0];
    this->layerObject->padding.assign(object["padding"].get<std::string>()); 
    this->layerObject->data_format.assign(object["data_format"].get<std::string>()); 
    this->layerObject->dilation_rate= object["dilation_rate"][0];
    this->layerObject->activation.assign(object["activation"].get<std::string>());
	    
    if (object["use_bias"] =="False"){
    	this->layerObject->use_bias = false;
    } else this->layerObject->use_bias = true;
    
    layerMap[this->layerObject->identifier] = this->layerObject;

    std::cout<< "LAYER_BUILDER: Conv1D layer "<< this->layerObject->identifier << " recognized"<<std::endl;
}



void InputBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){
//    json object = obj["config"];
    
   // TODO: Figure out if I actually need this 


}


void Conv2DBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){
    json object= obj["config"];
    int dim_vars= 2;
    this->layerObject->identifier.assign(id);
    
    if (object["batch_input_shape"].size() > 0){
        for (int i=0; i<(dim_vars+1); i++){
            this->layerObject->input_shape.push_back(object["batch_input_shape"][1+i]);
        }
    }  

    for (int i=0; i<dim_vars; i++){
        this->layerObject->kernel_size.push_back(object["kernel_size"][i]);
        this->layerObject->strides.push_back(object["strides"][i]);
        this->layerObject->dilation_rate.push_back(object["dilation_rate"][i]);
    }

    this->layerObject->filters = object["filters"];
    this->layerObject->padding.assign(object["padding"].get<std::string>());
    this->layerObject->data_format.assign(object["data_format"].get<std::string>());
    this->layerObject->activation.assign(object["activation"].get<std::string>());
    this->layerObject->use_bias= object["use_bias"];
    if (object["use_bias"] =="False"){
    	this->layerObject->use_bias = false;
    } else this->layerObject->use_bias = true;
    layerMap[this->layerObject->identifier] = this->layerObject;

    std::cout<< "LAYER_BUILDER: Conv2D layer " << this->layerObject->identifier << " recognized"<<std::endl;
}



void MaxPooling1DBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){

    json object= obj["config"];
    int dim_vars = 2;
    if (object["batch_input_shape"].size() > 0){ // this layer is the input layer
        for (int i=0; i<(dim_vars);i++){
            this->layerObject->input_shape.push_back(object["batch_input_shape"][i+1]);
        }
    }  

    this->layerObject->identifier.assign(id);
    this->layerObject->pool_size= object["pool_size"][0];
    this->layerObject->strides = object["strides"][0];
    this->layerObject->padding.assign(object["padding"].get<std::string>());
    this->layerObject->data_format.assign(object["data_format"].get<std::string>());
 
    layerMap[this->layerObject->identifier] = this->layerObject;
    std::cout<< "LAYER_BUILDER: MaxPooling1D layer " << this->layerObject->identifier << " recognized"<<std::endl;
}


void MaxPooling2DBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){
    json object= obj["config"];
    int dim_vars= 2; 
    if (object["batch_input_shape"].size() > 0){
        for (int i=0; i<(dim_vars+1); i++){
            this->layerObject->input_shape.push_back(object["batch_input_shape"][i+1]);
        }
    } 

    this->layerObject->identifier.assign(id);
    for (int i=0; i<dim_vars; i++){
        this->layerObject->pool_size.push_back(object["pool_size"][i]);
        this->layerObject->strides.push_back(object["strides"][i]);
    }
    this->layerObject->padding.assign(object["padding"].get<std::string>());
    this->layerObject->data_format.assign(object["data_format"].get<std::string>());
   
    layerMap[this->layerObject->identifier] = this->layerObject;
    std::cout<< "LAYER_BUILDER: MaxPooling2D layer " << this->layerObject->identifier << " recognized"<<std::endl;
   
}



void DenseBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){
    json object= obj["config"];
    this->layerObject->identifier.assign(id);
    this->layerObject->units=object["units"];
    this->layerObject->activation.assign(object["activation"].get<std::string>());
    this->layerObject->use_bias = object["use_bias"];
    
    if (object["batch_input_shape"].size() > 0){  // if this layer is the input layer
            for (int i=0; i<object["batch_input_shape"].size()-1; i++){
                this->layerObject->input_shape.push_back(object["batch_input_shape"][i+1]);
            }
    }
    
    if (object["use_bias"] =="False"){
    	this->layerObject->use_bias = false;
    } else this->layerObject->use_bias = true;

    this->layerObject->output_shape.push_back(this->layerObject->units);
    layerMap[this->layerObject->identifier] = this->layerObject;
    std::cout<< "LAYER_BUILDER: Dense layer " << this->layerObject->identifier << " recognized"<<std::endl;
}


void SimpleRNNBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){
    
    json object= obj["config"];
    this->layerObject->identifier.assign(id);
    this->layerObject->units=object["units"];
    this->layerObject->activation.assign(object["activation"].get<std::string>());
    this->layerObject->return_sequences= object["return_sequences"];
    this->layerObject->return_state = object["return_state"];
    this->layerObject->go_backwards = object["go_backwards"];
    this->layerObject->stateful = object["stateful"];
        
        if (object["batch_input_shape"].size() > 0){ // this layer is the input layer
           for (int i=0; i<object["batch_input_shape"].size() - 1; i++){
                this->layerObject->input_shape.push_back(object["batch_input_shape"][i+1]);
        }
    }

    if (object["use_bias"] =="False"){
    	this->layerObject->use_bias = false;
    } else this->layerObject->use_bias = true;
   
    this->layerObject->output_shape.push_back(this->layerObject->units);
    
    layerMap[this->layerObject->identifier] = this->layerObject;
    
    std::cout << "LAYER_BUILDER: SimpleRNN layer " << this->layerObject->identifier << " recognized" << std::endl;
}


void FlattenBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){
    this->layerObject->identifier.assign(id);
    layerMap[this->layerObject->identifier] = this->layerObject;
    std::cout<< "LAYER_BUILDER: Flatten layer " << this->layerObject->identifier << " recognized"<<std::endl;
}


void GRUBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){
    json object = obj["config"];
    this->layerObject->identifier.assign(id);
    this->layerObject->units = object["units"];
    this->layerObject->activation.assign(object["activation"].get<std::string>());
    this->layerObject->recurrent_activation.assign(object["recurrent_activation"].get<std::string>());
    this->layerObject->use_bias = object["use_bias"];
    this->layerObject->dropout = object["dropout"];
    this->layerObject->recurrent_dropout= object["recurrent_dropout"];
    this->layerObject->go_backwards=object["go_backwards"];
    this->layerObject->recurrent_activation.assign(object["recurrent_activation"].get<std::string>());
    
    this->layerObject->stateful = object["stateful"];
    this->layerObject->unroll = object["unroll"];
    this->layerObject->reset_after = object["reset_after"];
    
    layerMap[this->layerObject->identifier] = this->layerObject;
    if (object["batch_input_shape"].size() > 0){ // this layer is the input layer
           for (int i=0; i<object["batch_input_shape"].size() - 1; i++){
                this->layerObject->input_shape.push_back(object["batch_input_shape"][i + 1]);
            }
    }    
    if (object["use_bias"] =="False"){
    	this->layerObject->use_bias = false;
    } else this->layerObject->use_bias = true;
    std::cout << "LAYER_BUILDER: GRU layer " << this->layerObject->identifier << " recognized" << std::endl;
}

void LSTMBuilder::create_from_json(json obj, std::string id, std::map<std::string, Layer*>& layerMap){
    json object = obj["config"];
    this->layerObject->units = object["units"];
    this->layerObject->identifier.assign(id);
    this->layerObject->activation.assign(object["activation"].get<std::string>());
    
    this->layerObject->recurrent_activation.assign(object["recurrent_activation"].get<std::string>());
    this->layerObject->dropout = object["dropout"];
    
    this->layerObject->recurrent_dropout = object["recurrent_dropout"];
    this->layerObject->implementation = object["implementation"];
    this->layerObject->go_backwards = object["go_backwards"];
    this->layerObject->stateful = object["stateful"];
    this->layerObject->unroll = object["unroll"];

    if (object["batch_input_shape"].size() > 0){ // this layer is the input layer
           for (int i=0; i<object["batch_input_shape"].size() - 1; i++){
                this->layerObject->input_shape.push_back(object["batch_input_shape"][i + 1]);
            }
    }
    
    if (object["use_bias"] =="False"){
    	this->layerObject->use_bias = false;
    } else this->layerObject->use_bias = true;

    layerMap[this->layerObject->identifier] = this->layerObject;

    std::cout << "LAYER_BUILDER: LSTM layer " << this->layerObject->identifier << " recognized" << std::endl;
}


    
      

    
