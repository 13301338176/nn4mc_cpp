#include "generator/weight_generator.h"
#include "generator/tensor_representation.h"

#include <fstream>
#include <string>
#include <stdexcept>

std::string WeightGenerator::TEMPLATE_BEGIN_DELIMITER =	"<%BEGIN_TEMPLATE>";
std::string WeightGenerator::TEMPLATE_END_DELIMITER =	"<%END_TEMPLATE>";
std::string WeightGenerator::DATATYPE_DELIMITER =		"<%WEIGHT_DATATYPE>";
std::string WeightGenerator::NAME_DELIMITER =			"<%WEIGHT_NAME>";
std::string WeightGenerator::INDEX_DELIMITER =			"<%WEIGHT_INDEX>";
std::string WeightGenerator::DATA_DELIMITER =			"<%DATA>";

/*******************
* WeightGenerator(std::string template_path)
*
* Create a new object for generating C code for weight representation.
*
* Arguments:
*   template_path - Path to the file acting as a template for the C code.
*/
WeightGenerator::WeightGenerator(std::string template_path, bool flatten)
{
	// Load the template from the provided path
	std::ifstream template_file(template_path, std::ios::in);

	// If it's open, load the text
	if(template_file.is_open())
	{
		weight_template.assign ( (std::istreambuf_iterator<char>(template_file)), (std::istreambuf_iterator<char>()) );

		// Split the weight template into the template header, contents and footer
		// Get the header
		size_t begin_position = weight_template.find(TEMPLATE_BEGIN_DELIMITER);
		if(begin_position == std::string::npos)		{ /* What to do if the delimiter isn't found */ }
		template_header = weight_template.substr(0, begin_position);
		weight_template.erase(0, begin_position + TEMPLATE_BEGIN_DELIMITER.length());

		// Get the template
		size_t end_position = weight_template.find(TEMPLATE_END_DELIMITER);
		if(end_position == std::string::npos)		{ /* What to do if the delimiter isn't found */ }
		template_contents = weight_template.substr(0, end_position);
		weight_template.erase(0, end_position + TEMPLATE_END_DELIMITER.length());

		// Get the footer
		template_footer = std::string(weight_template);

		template_file.close();
	}
	else		// File did not open!
	{
		throw std::runtime_error("Could not open file: " + template_path);
	}

	// Store whether to flatten the indices
	flatArray = flatten;

	// Make an object to generate representation of tensors
	if(flatArray)
	{
		tensorRepresentation = new TensorRepresentation(" ","");
	}
	else
	{
		tensorRepresentation = new TensorRepresentation("{","}");
	}
}


/*******************
* ~WeightGenerator()
*
* Clean up the object by deleting the tensorRepresentation object.
*/
WeightGenerator::~WeightGenerator()
{
    // This line was causing a deallocation problem in generator_test_v2. 
    // Maybe let's just try to delete the weight generator
    // object instead of the tensor representation
	//delete tensorRepresentation;
}


/*******************
* replaceDelimiter(std::string* content, std::string delimiter, std::string value)
*
* Replaces the delimiter in the content with the provided value.
*
* Arguments:
*   content   - pointer to a string which the delimiter should be replaced with the value.
*   delimiter - the delimiter to be replaced.
*   value     - the value to replace the delimiter with.
*/
void WeightGenerator::replaceDelimiter(std::string* content, std::string delimiter, std::string value)
{
	// Where is the position of the delimiter?
	size_t delimiter_position = content->find(delimiter);

	// If no delimiter, simply return
	if(delimiter_position == std::string::npos)		return;

	// Otherwise, replace the delimiter with the provided string
	content->replace(delimiter_position, delimiter.length(), value);
}



/*******************
* getIndexRepresentation()
*
* Create a string representation of the indexing of the array.
*/
std::string WeightGenerator::getIndexRepresentation(Tensor<double> values)
{
	std::string indexRepresentation;


	// If we are to generate a flat array, dump the sum of all the weights,
	// otherwise, produce comma separated array
	if(flatArray)
	{
		indexRepresentation += "[" + std::to_string(values.num_elements) + "]";
	}
	else
	{
		for(int i=0; i<values.shape.size(); i++)
		{
			indexRepresentation += "[" + std::to_string(values.shape[i]) + "]";
		}
	}

	return indexRepresentation;
}


/*******************
* addWeight(Weight* weight)
*
* Create a string representation of a provided weight to be placed in the
* generated C code.
*
* Arguments:
*   weight - pointer to a weight object to add.
*/
void WeightGenerator::addWeight(Weight* weight)
{
	// Pull out the weight's tensor for simplicity
	Tensor<double> values = *(weight->get_weight_tensor());
	// Make a copy of the content template
	
	std::string content(template_contents);
	// Replace the datatype delimiter with the defined datatype

	replaceDelimiter(&content, DATATYPE_DELIMITER, weight_datatype);

	// Replace the data name delimiter with this weight's id
	//std::cout << weight->identifier << std::endl;
	replaceDelimiter(&content, NAME_DELIMITER, weight->identifier);

	// Figure out what the index should look like, and then place in the delimiter
	std::string index_string; //= getIndexRepresentation(values);

	// If we are to generate a flat array, dump the sum of all the weights,
	// otherwise, produce comma separated array
	if(flatArray)
	{
		index_string += "[" + std::to_string(values.num_elements) + "]";
	}
	else
	{
		for(int i=0; i<values.shape.size(); i++)
		{
			index_string += "[" + std::to_string(values.shape[i]) + "]";
		}
	}
    
    this->total_parameters += values.num_elements; 

	replaceDelimiter(&content, INDEX_DELIMITER, index_string);

	// Finally, generate the contents of the tensor
	size_t data_position = content.find(DATA_DELIMITER);

	// Before adding new lines, pad out the line so that the data contents are in the same column
	tensorRepresentation->addPad((unsigned int) content.find(DATA_DELIMITER));
	std::string data_string = tensorRepresentation->getString(weight->get_weight_tensor());
	replaceDelimiter(&content, DATA_DELIMITER, data_string);
	// And push this into the contents
	contents.push_back(content);
}


/*******************
* dump(std::string filename)
*
* Write the actual C code to a file using the current contents of the object.
*
* Arguments:
*   filename - path to the file to write to
*/
void WeightGenerator::dump(std::string filename)
{
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "GENERATOR: # Parameters = " << this->total_parameters << std::endl;
    std::cout << "GENERATOR: Flash Mem. Requirement: " << (this->total_parameters * sizeof(float)) <<  " Bytes " << std::endl;
	// Load the template from the provided path
	std::ofstream output_file(filename, std::ios::out);

	// If it's open, load the text
	if(output_file.is_open())
	{
		// Write the header
		output_file << template_header;

		// Write out each tensor representation
		for(std::list<std::string>::iterator iter=contents.begin(); iter != contents.end(); ++iter)
		{
			output_file << *iter;
		}

		// Write the footer
		output_file << template_footer;

		output_file.close();
	}
	else		// File did not open!
	{
		throw std::runtime_error("Could not open file: " + filename);
	}
}
