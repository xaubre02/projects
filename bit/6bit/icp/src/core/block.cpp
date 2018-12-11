/**
 * @file    block.cpp
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Základ pro tvorbu funkčních bloků.
 */

#include "block.h"


namespace core {
  // --------------------------- PORT --------------------------
    Port::Port(int type, std::string name) {
        this->type = type;
        this->name = name;
        // Výchozí hodnoty
        value = 0.0;
        pairPort = nullptr;
        customizable = true;
        valSet = false;
    
    }
    
    void Port::setValue(double value) {
        this->value = value;
        // pouze 1 nebo 0
        if (name == "boolean"){
            if (value == 0.0);
            else value = 1.0;
        }
        valSet = true;
    }

    bool Port::isAvailable() {
        // Na výstup se může připojit libovolný počet zařízení.
        if (type == PORT_OUT)
            return true;
        // Pokud už tento vstupní port není propojený, je dostupný.
        if (pairPort == nullptr)
            return true;
        // Jinak je nedostupný
        return false;
    }

    void Port::setPairPort(Port *port) { 
        pairPort = port;
        customizable = false;
        valSet = true; 
    }
    
    void Port::release() {
        value = 0.0;
        pairPort = nullptr;
        customizable = true;
        valSet = false; 
    }

  // -------------------------- BLOCK --------------------------
    Port *Block::inputPort(size_t id) {
        if(inPorts.size() > id)
            return inPorts[id];
        else
            return nullptr;
    }

    void Block::addInputPort(Port *port) {
        port->setIndex(inPorts.size());
        inPorts.push_back(port);
    }

    Block::~Block() {
        delete outPort;
        for(auto port : inPorts)
            delete port;
    }

    bool Block::connectTo(Block *block, int id) {
        Port *targetPort = block->inputPort(id);
        // Cílový blok musí mít daný vstupní port, který musí být dostupný.
        if (targetPort == nullptr || !targetPort->isAvailable())
            return false;
        // Výstupní port se musí napojit na vstupní port. (melo by byt vzdy)
        if (targetPort->portType() != PORT_IN)
            return false;
        // Porty musí mít stejný typ.
        if (targetPort->portName() != outPort->portName())
            return false;
        // nastaveni paroveho portu
        targetPort->setPairPort(outPort);
        outBlocks.push_back(block);
        return true;
    }

    void Block::releaseInputPort(int ID) {
        for (auto port : inPorts) {
            // Odpojení vstupního portu
            if (port->index() == ID)
                port->release();
        }
    }
    
    void Block::releaseOutputBlock(Block *block) {
        int id = 0;
        for (auto b: outBlocks) {
            if (b == block) {
                outBlocks.erase(outBlocks.begin() + id);
                return;
            }
            id++;
        }
    }

    bool Block::inputPortsConnected() {
        // Všechny vstupní porty musí být propojeny
        for (auto port : inPorts) {
            if (port->isAvailable())
                return false;
        }
        return true;
    }

    bool Block::outputPortConnected() {
        return (outBlocks.size() > 0);
    }

    void Block::update() {
        for (auto port : inPorts) {
            // Port musí být propojený -> není dostupný
            if (!port->isAvailable())
                port->setValue(port->connectedPort()->portValue());
        }
    }
    
    void Block::startCalculation() {
        if (calculate()) {
            for (auto block : outBlocks) {
                block->startCalculation();
            }
        }
    }

    bool Block::isInputBlock() {
        for (auto port : inPorts) {
            // na vstupni porty nesmi byt pripojen vystup jineho bloku
            if (!port->hasCustomizableValue())
                return false;
        }
        return true;
    }

    bool Block::isGoingToCycle() {
        std::vector<Block *> blocks;
        bool found = false;
        findCycle(blocks, found);
        return found;
    }
    
    void Block::findCycle(std::vector<Block *> &blocks, bool &found) {
        // cyklus jiz nalezen
        if (found) {
            return;
        } else {
            // pocitadlo vyskytu aktualniho bloku v jiz projitych blocich
            size_t cnt = 0;
            // pocet kombinaci cest, kterymi by se dalo dojit k tomuto bloku
            size_t combos = 1;
            for (auto block : blocks) {
                // pocitani vyskytu
                if (block == this) {
                    cnt++;
                }
                // pocitani kombinaci
                if (cnt == 0) {
                    combos *= block->getInputPorts().size();
                }
            }
            // pokud se tam nachazi vicekrat, nez je pocet kombinaci, kterymi by se dalo dojit k tomuto bloku, je zacyklen
            if (cnt > combos) {
                found = true;
                return;
            }
            else {
                // projiti bloku, na ktere se pripojuje aktualni blok
                blocks.push_back(this);
                for (auto block : outBlocks) {
                    block->findCycle(blocks, found);
                }
            }
        }
    }
}
