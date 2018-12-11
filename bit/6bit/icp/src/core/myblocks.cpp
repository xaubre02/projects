/**
 * @file    myblocks.cpp
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Vlastní funkční bloky.
 */

#include "myblocks.h"
#include <math.h>


namespace core {
    // --------------- NOT ----------------
    Not::Not() {
        setBlockName("Not");
        addInputPort(new Port(PORT_IN, "real"));
        setOutputPort(new Port(PORT_OUT, "real"));
    }

    bool Not::calculate() {
        update();
        double value = inputPort(0)->portValue() * -1;
        getOutputPort()->setValue(value);
        return true;
    }

    // --------------- SINE ---------------
    Sine::Sine() {
        setBlockName("Sine");
        addInputPort(new Port(PORT_IN, "real"));
        setOutputPort(new Port(PORT_OUT, "real"));
    }

    bool Sine::calculate() {
        update();
        double value = sin(inputPort(0)->portValue());
        getOutputPort()->setValue(value);
        return true;
    }

    // -------------- COSINE --------------
    Cosine::Cosine() {
        setBlockName("Cosine");
        addInputPort(new Port(PORT_IN, "real"));
        setOutputPort(new Port(PORT_OUT, "real"));
    }

    bool Cosine::calculate() {
        update();
        double value = cos(inputPort(0)->portValue());
        getOutputPort()->setValue(value);
        return true;
    }

    // --------------- ADDER --------------
    Adder::Adder() {
        setBlockName("Adder");
        addInputPort(new Port(PORT_IN, "real"));
        addInputPort(new Port(PORT_IN, "real"));
        setOutputPort(new Port(PORT_OUT, "real"));
    }

    bool Adder::calculate() {
        update();
        double value = inputPort(0)->portValue() + inputPort(1)->portValue();
        getOutputPort()->setValue(value);
        return true;
    }

    // ------------ SUBTRACTOR ------------
    Subtractor::Subtractor() {
        setBlockName("Subtractor");
        addInputPort(new Port(PORT_IN, "real"));
        addInputPort(new Port(PORT_IN, "real"));
        setOutputPort(new Port(PORT_OUT, "real"));
    }

    bool Subtractor::calculate() {
        update();
        double value = inputPort(0)->portValue() - inputPort(1)->portValue();
        getOutputPort()->setValue(value);
        return true;
    }

    // ------------ MULTIPLIER ------------
    Multiplier::Multiplier() {
        setBlockName("Multiplier");
        addInputPort(new Port(PORT_IN, "real"));
        addInputPort(new Port(PORT_IN, "real"));
        setOutputPort(new Port(PORT_OUT, "real"));
    }

    bool Multiplier::calculate() {
        update();
        double value = inputPort(0)->portValue() * inputPort(1)->portValue();
        getOutputPort()->setValue(value);
        return true;
    }

    // -------------- DIVIDER -------------
    Divider::Divider() {
        setBlockName("Divider");
        addInputPort(new Port(PORT_IN, "real"));
        addInputPort(new Port(PORT_IN, "real"));
        setOutputPort(new Port(PORT_OUT, "real"));
    }

    bool Divider::calculate() {
        update();
        // Dělení nulou
        if (!inputPort(1)->portValue()) 
            return false;

        double value = inputPort(0)->portValue() / inputPort(1)->portValue();
        getOutputPort()->setValue(value);
        return true;
    }

    // ------------ Logical AND -----------
    LogicalAND::LogicalAND() {
        setBlockName("LogicalAND");
        addInputPort(new Port(PORT_IN, "boolean"));
        addInputPort(new Port(PORT_IN, "boolean"));
        setOutputPort(new Port(PORT_OUT, "boolean"));
    }

    bool LogicalAND::calculate() {
        update();
        // oba vstupy musi byt v 1, aby ve vysledku byla 1
        if (inputPort(0)->portValue() && inputPort(1)->portValue())
            getOutputPort()->setValue(1.0);
        else
            getOutputPort()->setValue(0.0);
        return true;
    }

    // ------------- Logical OR -----------
    LogicalOR::LogicalOR() {
        setBlockName("LogicalOR");
        addInputPort(new Port(PORT_IN, "boolean"));
        addInputPort(new Port(PORT_IN, "boolean"));
        setOutputPort(new Port(PORT_OUT, "boolean"));
    }

    bool LogicalOR::calculate() {
        update();
        // alespon jeden vstup musi byt v 1, aby ve vysledku byla 1
        if (inputPort(0)->portValue() || inputPort(1)->portValue())
            getOutputPort()->setValue(1.0);
        else
            getOutputPort()->setValue(0.0);
        return true;
    }

    // ------------ Logical XOR -----------
    LogicalXOR::LogicalXOR() {
        setBlockName("LogicalXOR");
        addInputPort(new Port(PORT_IN, "boolean"));
        addInputPort(new Port(PORT_IN, "boolean"));
        setOutputPort(new Port(PORT_OUT, "boolean"));
    }

    bool LogicalXOR::calculate() {
        update();
        // vstupni hodnoty nesmi byt stejne
        if (inputPort(0)->portValue() == inputPort(1)->portValue())
            getOutputPort()->setValue(0.0);
        else
            getOutputPort()->setValue(1.0);
        return true;
    }

    // ------------ Logical NOT -----------
    LogicalNOT::LogicalNOT() {
        setBlockName("LogicalNOT");
        addInputPort(new Port(PORT_IN, "boolean"));
        setOutputPort(new Port(PORT_OUT, "boolean"));
    }

    bool LogicalNOT::calculate() {
        update();
        // negace vstupu
        if (inputPort(0)->portValue())
            getOutputPort()->setValue(0.0);
        else
            getOutputPort()->setValue(1.0);
        return true;
    }
}
