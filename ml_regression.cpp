//
//  ml_regression.cpp
//  ml
//
//  Created by Jamie Bullock on 25/02/2014.
//
//

#include "ml_regression.h"

namespace ml
{
    // Attribute setters
    void ml_regression::set_max_iterations(int max_iterations)
    {
        GRT::Regressifier &regressifier = get_Regressifier_instance();
        regressifier.setMaxNumEpochs(max_iterations);
    }
    
    void ml_regression::set_min_change(float min_change)
    {
        GRT::Regressifier &regressifier = get_Regressifier_instance();
        bool success = regressifier.setMinChange(min_change);
        
        if (success == false)
        {
            error("unable to set min_change, hint: should be greater than 0");
        }
    }
    
    void ml_regression::set_training_rate(float training_rate)
    {
        GRT::Regressifier &regressifier = get_Regressifier_instance();
        bool success = regressifier.setLearningRate(training_rate);
        
        if (success == false)
        {
            error("unable to set training_rate, hint: should be between 0-1");
        }
    }
    
    // Attribute getters
    void ml_regression::get_max_iterations(int &max_iterations) const
    {
        const GRT::Regressifier &regressifier = get_Regressifier_instance();
        max_iterations = regressifier.getMaxNumEpochs();
    }
    
    void ml_regression::get_min_change(float &min_change) const
    {
        error("function not implemented");
        //        min_change = regressifier->getMinChange();
    }
    
    void ml_regression::get_training_rate(float &training_rate) const
    {
        const GRT::Regressifier &regressifier = get_Regressifier_instance();
        training_rate = regressifier.getLearningRate();
    }
    
    // Methods
    void ml_regression::train()
    {
        GRT::UINT numSamples = labelledRegressionData.getNumSamples();
        GRT::Regressifier &regressifier = get_Regressifier_instance();
        
        if (numSamples == 0)
        {
            error("no observations added, use 'add' to add training data");
            return;
        }
        
        bool success = false;
        success = regressifier.train(labelledRegressionData);
        
        if (!success)
        {
            error("training failed");
        }
        
        t_atom a_success;
        
        SetInt(a_success, success);
        ToOutAnything(1, s_train, 1, &a_success);
    }
    
    void ml_regression::map(int argc, const t_atom *argv)
    {
        GRT::UINT numSamples = labelledRegressionData.getNumSamples();
        GRT::Regressifier &regressifier = get_Regressifier_instance();
        
        if (numSamples == 0)
        {
            error("no observations added, use 'add' to add training data");
            return;
        }
        
        if (regressifier.getTrained() == false)
        {
            error("data_typel has not been trained, use 'train' to train the data_typel");
            return;
        }
        
        GRT::UINT numInputNeurons = regressifier.getNumInputFeatures();
        GRT::VectorDouble query(numInputNeurons);
        
        if (argc < 0 || (unsigned)argc != numInputNeurons)
        {
            error("invalid input length, expected %d, got %d", numInputNeurons, argc);
        }
        
        for (uint32_t index = 0; index < (uint32_t)argc; ++index)
        {
            double value = GetAFloat(argv[index]);
            query[index] = value;
        }
        
        bool success = regressifier.predict(query);
        
        if (success == false)
        {
            error("unable to map input");
            return;
        }
        
        GRT::VectorDouble regressionData = regressifier.getRegressionData();
        GRT::VectorDouble::size_type numOutputDimensions = regressionData.size();
        
        if (numOutputDimensions != regressifier.getNumOutputDimensions())
        {
            error("invalid output dimensions: %d", numOutputDimensions);
            return;
        }
        
        AtomList result;
        
        for (uint32_t index = 0; index < numOutputDimensions; ++index)
        {
            t_atom value_a;
            double value = regressionData[index];
            SetFloat(value_a, value);
            result.Append(value_a);
        }
        
        ToOutList(0, result);
    }
    
    void ml_regression::usage()
    {
        post("%s", ML_POST_SEP);
        post("Attributes:");
        post("%s", ML_POST_SEP);
        post("num_inputs:\tinteger setting number of neurons in the input layer of the MLP (default %d)", defaultNumInputDimensions);
        post("training_rate:\tfloating point value used to update the weights at each step of the stochastic gradient descent (default 0.1)");
        post("scaling:\tinteger (0 or 1) sets whether values are automatically scaled (default 1)");
        post("probs:\tinteger (0 or 1) determing whether probabilities are sent from the right outlet");
        post("%s", ML_POST_SEP);
        post("Methods:");
        post("%s", ML_POST_SEP);
        post("add:\tlist comprising a class id followed by n features; <class> <feature 1> <feature 2> etc");
        post("save:\tsave training examples, first argument gives path to save location");
        post("load:\tload training examples, first argument gives path to the load location");
        post("train:\ttrain the MLP based on vectors added with 'add'");
        post("clear:\tclear the stored training data and data_typel");
        post("map:\tgive the regression value for the input feature vector");
        post("help:\tpost this usage statement to the console");
        post("%s", ML_POST_SEP);
    }
    
#pragma mark - pure virtual method implementation
    GRT::MLBase &ml_regression::get_MLBase_instance()
    {
        return get_Regressifier_instance();
    }
    
    const GRT::MLBase &ml_regression::get_MLBase_instance() const
    {
        return get_Regressifier_instance();
    }
}