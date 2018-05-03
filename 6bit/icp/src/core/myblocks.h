/**
 * @file    myblocks.h
 * @author  Tomáš Aubrecht (xaubre02)
 * @date    květen 2018
 * @brief   Vlastní funkční bloky.
 */

#pragma once

#include "block.h"

/**
 * @brief Prostor jmen pro základní bloky bez GUI.
 */
namespace core {
    /**
     * @brief Funkční blok - negace
     */
    class Not final : public Block {
    public:
        /**
         * @brief konstruktor funkčního bloku
         */
        Not();
        /**
         * @brief virtuální destruktor funkčního bloku
         */
        virtual ~Not() {}
        /**
         * @brief provede výpočet funkčního bloku
         */
        virtual bool calculate() override;
    };

    /**
     * @brief Funkční blok - sinus
     */
    class Sine final : public Block {
    public:
        /**
         * @brief konstruktor funkčního bloku
         */
        Sine();
        virtual ~Sine() {}
        /**
         * @brief provede výpočet funkčního bloku
         */
        virtual bool calculate() override;
    };

    /**
     * @brief Funkční blok - cosinus
     */
    class Cosine final : public Block {
    public:
        /**
         * @brief konstruktor funkčního bloku
         */
        Cosine();
        /**
         * @brief virtuální destruktor funkčního bloku
         */
        virtual ~Cosine() {}
        /**
         * @brief provede výpočet funkčního bloku
         */
        virtual bool calculate() override;
    };

    /**
     * @brief Funkční blok - sčítání
     */
    class Adder final : public Block {
    public:
        /**
         * @brief konstruktor funkčního bloku
         */
        Adder();
        /**
         * @brief virtuální destruktor funkčního bloku
         */
        virtual ~Adder() {}
        /**
         * @brief provede výpočet funkčního bloku
         */
        virtual bool calculate() override;
    };

    /**
     * @brief Funkční blok - odčítání
     */
    class Subtractor final : public Block {
    public:
        /**
         * @brief konstruktor funkčního bloku
         */
        Subtractor();
        /**
         * @brief virtuální destruktor funkčního bloku
         */
        virtual ~Subtractor() {}
        /**
         * @brief provede výpočet funkčního bloku
         */
        virtual bool calculate() override;
    };

    /**
     * @brief Funkční blok - násobení
     */
    class Multiplier final : public Block {
    public:
        /**
         * @brief konstruktor funkčního bloku
         */
        Multiplier();
        /**
         * @brief virtuální destruktor funkčního bloku
         */
        virtual ~Multiplier() {}
        /**
         * @brief provede výpočet funkčního bloku
         */
        virtual bool calculate() override;
    };

    /**
     * @brief Funkční blok - dělení
     */
    class Divider final : public Block {
    public:
        /**
         * @brief konstruktor funkčního bloku
         */
        Divider();
        /**
         * @brief virtuální destruktor funkčního bloku
         */
        virtual ~Divider() {}
        /**
         * @brief provede výpočet funkčního bloku
         */
        virtual bool calculate() override;
    };

    /**
     * @brief Funkční blok - logický AND
     */
    class LogicalAND final : public Block {
    public:
        /**
         * @brief konstruktor funkčního bloku
         */
        LogicalAND();
        /**
         * @brief virtuální destruktor funkčního bloku
         */
        virtual ~LogicalAND() {}
        /**
         * @brief provede výpočet funkčního bloku
         */
        virtual bool calculate() override;
    };

    /**
     * @brief Funkční blok - logický OR
     */
    class LogicalOR final : public Block {
    public:
        /**
         * @brief konstruktor funkčního bloku
         */
        LogicalOR();
        /**
         * @brief virtuální destruktor funkčního bloku
         */
        virtual ~LogicalOR() {}
        /**
         * @brief provede výpočet funkčního bloku
         */
        virtual bool calculate() override;
    };

    /**
     * @brief Funkční blok - logický XOR
     */
    class LogicalXOR final : public Block {
    public:
        /**
         * @brief konstruktor funkčního bloku
         */
        LogicalXOR();
        /**
         * @brief virtuální destruktor funkčního bloku
         */
        virtual ~LogicalXOR() {}
        /**
         * @brief provede výpočet funkčního bloku
         */
        virtual bool calculate() override;
    };

    /**
     * @brief Funkční blok - logický NOT
     */
    class LogicalNOT final : public Block {
    public:
        /**
         * @brief konstruktor funkčního bloku
         */
        LogicalNOT();
        /**
         * @brief virtuální destruktor funkčního bloku
         */
        virtual ~LogicalNOT() {}
        /**
         * @brief provede výpočet funkčního bloku
         */
        virtual bool calculate() override;
    };
}
