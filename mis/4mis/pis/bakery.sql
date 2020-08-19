-- phpMyAdmin SQL Dump
-- version 4.8.5
-- https://www.phpmyadmin.net/
--
-- Počítač: 127.0.0.1
-- Vytvořeno: Sob 04. dub 2020, 23:12
-- Verze serveru: 10.1.38-MariaDB
-- Verze PHP: 7.3.4

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Databáze: `bakery`
--

-- --------------------------------------------------------

--
-- Struktura tabulky `user`
--

CREATE TABLE `user` (
  `USERID` int(11) NOT NULL,
  `BORN` date DEFAULT NULL,
  `EMAIL` varchar(128) COLLATE utf8mb4_czech_ci DEFAULT NULL,
  `NAME` varchar(255) COLLATE utf8mb4_czech_ci DEFAULT NULL,
  `PASSWORD` varchar(255) COLLATE utf8mb4_czech_ci DEFAULT NULL,
  `ROLE` varchar(255) COLLATE utf8mb4_czech_ci DEFAULT NULL,
  `SURNAME` varchar(255) COLLATE utf8mb4_czech_ci DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `user`
--

INSERT INTO `user` (`USERID`, `BORN`, `EMAIL`, `NAME`, `PASSWORD`, `ROLE`, `SURNAME`) VALUES
(1, '1995-11-12', 'manager1@bakery.cz', 'Vin', 'bakery', 'manager', 'Diesel'),
(2, '1995-11-13', 'manager2@bakery.cz', 'Jaromír', 'bakery', 'manager', 'Jágr'),
(3, '1996-01-03', 'manager3@bakery.cz', 'Sheldon', 'bakery', 'manager', 'Cooper'),
(4, '1990-02-03', 'pastrystoreman1@bakery.cz', 'Vít', 'bakery', 'pastrystoreman', 'Ambrož'),
(5, '1991-03-02', 'pastrystoreman2@bakery.cz', 'Tomáš', 'bakery', 'pastrystoreman', 'Aubrecht'),
(6, '1993-03-28', 'materialstoreman1@bakery.cz', 'David', 'bakery', 'materialstoreman', 'Bednařík'),
(7, '1994-06-06', 'materialstoreman2@bakery.cz', 'Martin', 'bakery', 'materialstoreman', 'Fišer'),
(8, '1995-01-02', 'baker1@bakery.cz', 'Lukáš', 'bakery', 'baker', 'Bílek'),
(9, '1997-10-04', 'baker2@bakery.cz', 'Roman', 'bakery', 'baker', 'Čihák'),
(10, '1989-09-05', 'baker3@bakery.cz', 'David', 'bakery', 'baker', 'Zich'),
(11, '1987-11-11', 'baker4@bakery.cz', 'Radim', 'bakery', 'baker', 'Kasal'),
(12, '1985-12-12', 'customer1@bakery.cz', 'Michal', 'bakery', 'customer', 'Horák'),
(13, '1995-12-09', 'customer2@bakery.cz', 'Roman', 'bakery', 'customer', 'Horák'),
(14, '1991-07-08', 'customer3@bakery.cz', 'Kamil', 'bakery', 'customer', 'Danihel'),
(15, '1990-08-07', 'customer4@bakery.cz', 'Daniel', 'bakery', 'customer', 'Staněk'),
(16, '1983-05-11', 'customer5@bakery.cz', 'Jiří', 'bakery', 'customer', 'Peřina'),
(17, '1988-03-07', 'customer6@bakery.cz', 'Lukáš', 'bakery', 'customer', 'Vencelides');


-- --------------------------------------------------------

--
-- Struktura tabulky `material`
--

CREATE TABLE `material` (
  `MATERIALID` int(11) NOT NULL,
  `NAME` varchar(255) COLLATE utf8mb4_czech_ci DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `material`
--

INSERT INTO `material` (`MATERIALID`, `NAME`) VALUES
(1, 'Mouka hladká'),
(2, 'Mouka polohrubá'),
(3, 'Mouka hrubá'),
(4, 'Mouka bezlepková'),
(5, 'Mouka celozrnná'),
(6, 'Mouka špaldová'),
(7, 'Sůl'),
(8, 'Škrob'),
(9, 'Cukr krystal'),
(10, 'Cukr moučka'),
(11, 'Cukr vanilkový'),
(12, 'Cukr skočicový'),
(13, 'Cukr třtinový'),
(14, 'Krupice'),
(15, 'Skořice'),
(16, 'Mléko'),
(17, 'Olej'),
(18, 'Ocet'),
(19, 'Máslo'),
(20, 'Sádlo'),
(21, 'Kmín'),
(22, 'Vejce'),
(23, 'Droždí'),
(24, 'Oříšky'),
(25, 'Jablka'),
(26, 'Hrozinky'),
(27, 'Banány'),
(28, 'Švestky'),
(29, 'Hrušky'),
(30, 'Jahody'),
(31, 'Třešně'),
(32, 'Mandle'),
(33, 'Semínka dýňová'),
(34, 'Semínka slunečnicová'),
(35, 'Semínka sezamová'),
(36, 'Slanina'),
(37, 'Mák'),
(38, 'Nugát'),
(39, 'Sýr Eidam'),
(40, 'Kečup'),
(41, 'Listové těsto');

-- --------------------------------------------------------

--
-- Struktura tabulky `materialorder`
--

CREATE TABLE `materialorder` (
  `ORDERID` int(11) NOT NULL,
  `DATEOFCREATION` date DEFAULT NULL,
  `DELIVERED` tinyint(1) DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `materialorder`
--

INSERT INTO `materialorder` (`ORDERID`, `DATEOFCREATION`, `DELIVERED`) VALUES
(1, '2020-04-02', 1),
(2, '2020-04-02', 1),
(3, '2020-04-03', 1),
(4, '2020-04-03', 1),
(5, '2020-04-03', 1),
(6, '2020-04-03', 1),
(7, '2020-04-03', 0),
(8, '2020-04-03', 0),
(9, '2020-04-03', 0),
(10, '2020-04-06', 0),
(11, '2020-04-06', 0),
(12, '2020-04-06', 0);

-- --------------------------------------------------------

--
-- Struktura tabulky `materialorderitem`
--

CREATE TABLE `materialorderitem` (
  `AMOUNT` double DEFAULT NULL,
  `COST` double DEFAULT NULL,
  `MATERIAL_MATERIALID` int(11) NOT NULL,
  `ORDER_ORDERID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `materialorderitem`
--

INSERT INTO `materialorderitem` (`AMOUNT`, `COST`, `MATERIAL_MATERIALID`, `ORDER_ORDERID`) VALUES
(240, 21.5, 1, 9),
(200, 19, 2, 9),
(150, 19, 3, 9),
(140, 18, 4, 9),
(50, 24, 5, 9),
(35, 29, 6, 9),
(100, 14, 7, 5),
(5, 28, 8, 5),
(40, 8.9, 9, 1),
(50, 10, 10, 1),
(15, 39, 11, 1),
(8, 14.5, 12, 1),
(10, 24, 13, 1),
(30, 22, 14, 12),
(4, 125, 15, 8),
(25, 24, 17, 4),
(10, 9.9, 18, 4),
(15, 38, 19, 10),
(10, 19, 20, 10),
(8, 42, 21, 12),
(40, 55, 22, 5),
(40, 60, 23, 11),
(8, 88, 24, 2),
(8, 99.9, 26, 6),
(5, 14, 27, 2),
(6, 40, 28, 8),
(10, 60, 29, 6),
(9, 45, 30, 2),
(6, 39, 31, 2),
(20, 62.5, 32, 8),
(15, 99, 33, 3),
(20, 120, 34, 3),
(10, 70, 35, 3),
(4, 100, 36, 4),
(30, 89, 37, 11),
(5, 130, 39, 5),
(10, 18, 40, 7);

-- --------------------------------------------------------

--
-- Struktura tabulky `materialstorage`
--

CREATE TABLE `materialstorage` (
  `AMOUNT` double DEFAULT NULL,
  `MATERIAL_MATERIALID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `materialstorage`
--

INSERT INTO `materialstorage` (`AMOUNT`, `MATERIAL_MATERIALID`) VALUES
(30, 1),
(22, 2),
(40, 3),
(22, 4),
(20, 6),
(10, 7),
(5, 8),
(40, 9),
(13, 10),
(10, 11),
(3, 12),
(5, 14),
(0, 15),
(40, 16),
(30, 17),
(5, 19),
(1, 20),
(3, 21),
(20, 22),
(10, 23),
(20, 25),
(0.5, 26),
(5, 28),
(3, 30),
(4, 32),
(5, 33),
(4, 34),
(8, 35),
(3, 36),
(4, 37),
(2, 38),
(9, 39),
(0.5, 40),
(3, 41);


-- --------------------------------------------------------

--
-- Struktura tabulky `pastrycategory`
--

CREATE TABLE `pastrycategory` (
  `CATEGORYID` int(11) NOT NULL,
  `NAME` varchar(255) COLLATE utf8mb4_czech_ci DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `pastrycategory`
--

INSERT INTO `pastrycategory` (`CATEGORYID`, `NAME`) VALUES
(1, 'Světlé pečivo'),
(2, 'Tmavé pečivo'),
(3, 'Sladké pečivo'),
(4, 'Bezlepkové pečivo'),
(5, 'Ostatní');

-- --------------------------------------------------------

--
-- Struktura tabulky `pastry`
--

CREATE TABLE `pastry` (
  `PASTRYID` int(11) NOT NULL,
  `COST` double DEFAULT NULL,
  `NAME` varchar(255) COLLATE utf8mb4_czech_ci DEFAULT NULL,
  `CATEGORY_CATEGORYID` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `pastry`
--

INSERT INTO `pastry` (`PASTRYID`, `COST`, `NAME`, `CATEGORY_CATEGORYID`) VALUES
(1, 1.5, 'Rohlík', 1),
(2, 6, 'Bageta světlá', 1),
(3, 13, 'Veka', 1),
(4, 19, 'Chléb pšeničný', 1),
(5, 3, 'Bulka', 1),
(6, 4.5, 'Houska', 1),
(7, 24.5, 'Chléb žitný', 2),
(8, 8, 'Bageta tmavá', 2),
(9, 3.5, 'Kaiserka tmavá', 2),
(10, 7, 'Dalamánek tmavý', 2),
(11, 4, 'Kobliha nugát', 3),
(12, 4.5, 'Kobliha ovocná', 3),
(13, 8, 'Hvězda skořicová', 3),
(14, 27, 'Vánočka', 3),
(15, 24, 'Závin jablečný', 3),
(16, 31, 'Závin makový', 3),
(17, 24, 'Chléb bezlepkový', 4),
(18, 6, 'Houska bezlepková', 4),
(19, 19, 'Veka bezlepková', 4),
(20, 6, 'Bulka bezlepková', 1),
(21, 9, 'Angličák', 5),
(22, 6, 'Slaninka', 5),
(23, 11, 'Pizza koláč', 5);

-- --------------------------------------------------------

--
-- Struktura tabulky `pastrymaterial`
--

CREATE TABLE `pastrymaterial` (
  `AMOUNT` double DEFAULT NULL,
  `MATERIAL_MATERIALID` int(11) NOT NULL,
  `PASTRY_PASTRYID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `pastrymaterial`
--

INSERT INTO `pastrymaterial` (`AMOUNT`, `MATERIAL_MATERIALID`, `PASTRY_PASTRYID`) VALUES
(0.06, 1, 1),
(0.15, 1, 2),
(0.4, 1, 3),
(0.8, 1, 4),
(0.08, 1, 5),
(0.12, 1, 6),
(0.08, 1, 11),
(0.08, 1, 12),
(0.13, 1, 21),
(0.08, 1, 22),
(0.75, 2, 7),
(0.08, 2, 8),
(0.25, 2, 10),
(0.08, 2, 13),
(0.6, 2, 14),
(0.65, 2, 15),
(0.55, 2, 16),
(0.09, 3, 9),
(0.7, 4, 17),
(0.14, 4, 18),
(0.45, 4, 19),
(0.08, 4, 20),
(0.004, 7, 1),
(0.01, 7, 2),
(0.005, 7, 3),
(0.02, 7, 4),
(0.005, 7, 5),
(0.004, 7, 6),
(0.005, 7, 7),
(0.005, 7, 8),
(0.005, 7, 9),
(0.01, 7, 10),
(0.002, 7, 11),
(0.002, 7, 12),
(0.002, 7, 13),
(0.001, 7, 14),
(0.002, 7, 15),
(0.001, 7, 16),
(0.02, 7, 17),
(0.004, 7, 18),
(0.005, 7, 19),
(0.005, 7, 20),
(0.01, 7, 21),
(0.004, 7, 22),
(0.005, 7, 23),
(0.02, 9, 11),
(0.02, 9, 12),
(0.01, 9, 14),
(0.09, 9, 15),
(0.1, 9, 16),
(0.01, 10, 11),
(0.01, 10, 12),
(0.04, 10, 15),
(0.005, 12, 13),
(0.004, 15, 13),
(0.05, 16, 1),
(0.04, 16, 2),
(0.25, 16, 3),
(0.7, 16, 4),
(0.1, 16, 5),
(0.1, 16, 6),
(0.35, 16, 7),
(0.1, 16, 8),
(8, 16, 9),
(0.1, 16, 10),
(0.05, 16, 11),
(0.07, 16, 12),
(0.05, 16, 13),
(0.3, 16, 14),
(0.3, 16, 15),
(0.3, 16, 16),
(0.4, 16, 17),
(0.09, 16, 18),
(0.2, 16, 19),
(0.1, 16, 20),
(0.15, 16, 21),
(0.09, 16, 22),
(0.05, 17, 11),
(0.05, 17, 12),
(0.02, 19, 5),
(0.03, 19, 11),
(0.02, 19, 14),
(0.04, 19, 15),
(0.05, 19, 16),
(0.002, 21, 7),
(0.02, 22, 1),
(0.05, 22, 2),
(0.15, 22, 3),
(0.2, 22, 4),
(0.05, 22, 5),
(0.1, 22, 6),
(0.2, 22, 7),
(0.05, 22, 8),
(0.06, 22, 9),
(0.04, 22, 10),
(0.04, 22, 11),
(0.05, 22, 12),
(0.04, 22, 13),
(0.05, 22, 14),
(0.2, 22, 15),
(0.15, 22, 16),
(0.25, 22, 17),
(0.06, 22, 18),
(0.15, 22, 19),
(0.05, 22, 20),
(0.05, 22, 21),
(0.05, 22, 22),
(0.02, 22, 23),
(0.01, 23, 1),
(0.02, 23, 2),
(0.02, 23, 3),
(0.1, 23, 4),
(0.02, 23, 5),
(0.03, 23, 6),
(0.06, 23, 7),
(0.02, 23, 8),
(0.02, 23, 9),
(0.01, 23, 10),
(0.02, 23, 11),
(0.02, 23, 12),
(0.01, 23, 13),
(0.02, 23, 14),
(0.01, 23, 15),
(0.02, 23, 16),
(0.2, 23, 17),
(0.01, 23, 18),
(0.02, 23, 19),
(0.02, 23, 20),
(0.04, 23, 21),
(0.02, 23, 22),
(0.15, 25, 15),
(0.04, 30, 12),
(0.04, 32, 14),
(0.02, 33, 20),
(0.02, 34, 10),
(0.04, 35, 5),
(0.1, 35, 8),
(0.01, 35, 9),
(0.02, 36, 21),
(0.04, 36, 22),
(0.04, 36, 23),
(0.2, 37, 16),
(0.03, 38, 11),
(0.1, 39, 21),
(0.02, 39, 22),
(0.04, 39, 23),
(0.02, 40, 23),
(0.1, 41, 23);

-- --------------------------------------------------------

--
-- Struktura tabulky `pastryorder`
--

CREATE TABLE `pastryorder` (
  `ORDERID` int(11) NOT NULL,
  `DELIVERYDATE` datetime DEFAULT NULL,
  `PRICE` double DEFAULT NULL,
  `STATE` varchar(255) COLLATE utf8mb4_czech_ci DEFAULT NULL,
  `CUSTOMER_USERID` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `pastryorder`
--

INSERT INTO `pastryorder` (`ORDERID`, `DELIVERYDATE`, `PRICE`, `STATE`, `CUSTOMER_USERID`) VALUES
(1, '2020-04-01 00:00:00', 158, 'delivered', 13),
(2, '2020-04-02 00:00:00', 168, 'delivered', 12),
(3, '2020-04-02 00:00:00', 157.5, 'canceled', 14),
(4, '2020-04-02 00:00:00', 510, 'delivered', 15),
(5, '2020-04-03 00:00:00', 163, 'canceled', 16),
(6, '2020-04-03 00:00:00', 190, 'delivered', 17),
(7, '2020-04-06 00:00:00', 236, 'delivered', 13),
(8, '2020-04-06 00:00:00', 107.5, 'delivered', 12),
(9, '2020-04-07 00:00:00', 176.5, 'prepared', 17),
(10, '2020-04-07 00:00:00', 296, 'prepared', 15),
(11, '2020-04-07 00:00:00', 121.5, 'prepared', 12),
(12, '2020-04-08 00:00:00', 110, 'created', 14),
(13, '2020-04-08 00:00:00', 151, 'canceled', 16),
(14, '2020-04-08 00:00:00', 146, 'created', 13),
(15, '2020-04-09 00:00:00', 157, 'created', 17),
(16, '2020-04-09 00:00:00', 95, 'created', 15),
(17, '2020-04-10 00:00:00', 141.5, 'created', 12);

-- --------------------------------------------------------

--
-- Struktura tabulky `pastryorderitem`
--

CREATE TABLE `pastryorderitem` (
  `COST` double DEFAULT NULL,
  `COUNT` int(11) DEFAULT NULL,
  `ORDER_ORDERID` int(11) NOT NULL,
  `PASTRY_PASTRYID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `pastryorderitem`
--

INSERT INTO `pastryorderitem` (`COST`, `COUNT`, `ORDER_ORDERID`, `PASTRY_PASTRYID`) VALUES
(48, 8, 1, 2),
(30, 10, 1, 5),
(80, 10, 1, 8),
(98, 4, 2, 7),
(70, 10, 2, 10),
(67.5, 15, 3, 6),
(90, 10, 3, 21),
(240, 10, 4, 17),
(120, 20, 4, 18),
(150, 25, 4, 20),
(30, 20, 5, 1),
(52, 4, 5, 3),
(81, 3, 5, 14),
(60, 40, 6, 1),
(60, 20, 6, 5),
(70, 20, 6, 9),
(76, 4, 7, 4),
(105, 15, 7, 10),
(55, 5, 7, 23),
(40, 10, 8, 11),
(67.5, 15, 8, 12),
(37.5, 25, 9, 1),
(26, 2, 9, 3),
(49, 2, 9, 7),
(64, 8, 9, 13),
(80, 10, 10, 8),
(70, 20, 10, 9),
(98, 14, 10, 10),
(48, 2, 10, 15),
(37.5, 25, 11, 1),
(64, 8, 11, 8),
(20, 5, 11, 11),
(19, 1, 12, 4),
(36, 6, 12, 22),
(55, 5, 12, 23),
(90, 15, 13, 2),
(31, 1, 13, 16),
(30, 5, 13, 22),
(24, 1, 14, 17),
(38, 2, 14, 19),
(84, 14, 14, 20),
(30, 10, 15, 5),
(54, 12, 15, 6),
(49, 2, 15, 7),
(24, 1, 15, 15),
(35, 5, 16, 10),
(36, 8, 16, 12),
(24, 1, 16, 15),
(30, 20, 17, 1),
(38, 2, 17, 4),
(73.5, 3, 17, 7);

-- --------------------------------------------------------

--
-- Struktura tabulky `pastrystorage`
--

CREATE TABLE `pastrystorage` (
  `COUNT` int(11) DEFAULT NULL,
  `PASTRY_PASTRYID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `pastrystorage`
--

INSERT INTO `pastrystorage` (`COUNT`, `PASTRY_PASTRYID`) VALUES
(250, 1),
(50, 2),
(30, 3),
(90, 4),
(45, 5),
(150, 6),
(80, 7),
(10, 8),
(20, 10),
(50, 11),
(100, 12),
(50, 13),
(18, 14),
(15, 15),
(10, 16),
(30, 17),
(80, 18),
(40, 20),
(30, 21),
(20, 22);

-- --------------------------------------------------------
-- --------------------------------------------------------

--
-- Struktura tabulky `productionplan`
--

CREATE TABLE `productionplan` (
  `PLANID` int(11) NOT NULL,
  `ACCEPTED` tinyint(1) DEFAULT '0',
  `PRODUCTIONDATE` datetime DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `productionplan`
--

INSERT INTO `productionplan` (`PLANID`, `ACCEPTED`, `PRODUCTIONDATE`) VALUES
(1, 1, '2020-04-02 00:00:00'),
(2, 1, '2020-04-03 00:00:00'),
(3, 1, '2020-04-06 00:00:00'),
(4, 1, '2020-04-07 00:00:00'),
(5, 1, '2020-04-08 00:00:00'),
(6, 0, '2020-04-09 00:00:00'),
(7, 0, '2020-04-10 00:00:00');

-- --------------------------------------------------------

--
-- Struktura tabulky `productionplanitem`
--

CREATE TABLE `productionplanitem` (
  `COUNT` int(11) DEFAULT NULL,
  `PASTRY_PASTRYID` int(11) NOT NULL,
  `PLAN_PLANID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_czech_ci;

--
-- Vypisuji data pro tabulku `productionplanitem`
--

INSERT INTO `productionplanitem` (`COUNT`, `PASTRY_PASTRYID`, `PLAN_PLANID`) VALUES
(120, 1, 1),
(80, 1, 3),
(20, 1, 6),
(50, 2, 1),
(5, 2, 2),
(15, 2, 3),
(25, 2, 7),
(100, 4, 1),
(25, 4, 4),
(2, 4, 6),
(40, 5, 1),
(40, 5, 3),
(20, 5, 5),
(10, 6, 2),
(24, 6, 5),
(10, 7, 5),
(3, 7, 6),
(20, 8, 3),
(15, 10, 5),
(12, 12, 5),
(20, 13, 7),
(20, 14, 3),
(10, 15, 4),
(10, 15, 5),
(10, 16, 4),
(30, 17, 1),
(20, 17, 4),
(12, 19, 4),
(8, 19, 5),
(25, 20, 3),
(15, 20, 4),
(40, 21, 3),
(20, 21, 7),
(20, 22, 4),
(25, 23, 4),
(45, 23, 7);


--
-- Klíče pro exportované tabulky
--

--
-- Klíče pro tabulku `material`
--
ALTER TABLE `material`
  ADD PRIMARY KEY (`MATERIALID`);

--
-- Klíče pro tabulku `materialorder`
--
ALTER TABLE `materialorder`
  ADD PRIMARY KEY (`ORDERID`);

--
-- Klíče pro tabulku `materialorderitem`
--
ALTER TABLE `materialorderitem`
  ADD PRIMARY KEY (`MATERIAL_MATERIALID`,`ORDER_ORDERID`),
  ADD KEY `FK_MaterialOrderItem_ORDER_ORDERID` (`ORDER_ORDERID`);

--
-- Klíče pro tabulku `materialstorage`
--
ALTER TABLE `materialstorage`
  ADD PRIMARY KEY (`MATERIAL_MATERIALID`);

--
-- Klíče pro tabulku `pastry`
--
ALTER TABLE `pastry`
  ADD PRIMARY KEY (`PASTRYID`),
  ADD KEY `FK_Pastry_CATEGORY_CATEGORYID` (`CATEGORY_CATEGORYID`);

--
-- Klíče pro tabulku `pastrycategory`
--
ALTER TABLE `pastrycategory`
  ADD PRIMARY KEY (`CATEGORYID`);

--
-- Klíče pro tabulku `pastrymaterial`
--
ALTER TABLE `pastrymaterial`
  ADD PRIMARY KEY (`MATERIAL_MATERIALID`,`PASTRY_PASTRYID`),
  ADD KEY `FK_PastryMaterial_PASTRY_PASTRYID` (`PASTRY_PASTRYID`);

--
-- Klíče pro tabulku `pastryorder`
--
ALTER TABLE `pastryorder`
  ADD PRIMARY KEY (`ORDERID`),
  ADD KEY `FK_PastryOrder_CUSTOMER_USERID` (`CUSTOMER_USERID`);

--
-- Klíče pro tabulku `pastryorderitem`
--
ALTER TABLE `pastryorderitem`
  ADD PRIMARY KEY (`ORDER_ORDERID`,`PASTRY_PASTRYID`),
  ADD KEY `FK_PastryOrderItem_PASTRY_PASTRYID` (`PASTRY_PASTRYID`);

--
-- Klíče pro tabulku `pastrystorage`
--
ALTER TABLE `pastrystorage`
  ADD PRIMARY KEY (`PASTRY_PASTRYID`);

--
-- Klíče pro tabulku `productionplan`
--
ALTER TABLE `productionplan`
  ADD PRIMARY KEY (`PLANID`),
  ADD UNIQUE KEY `PRODUCTIONDATE` (`PRODUCTIONDATE`);

--
-- Klíče pro tabulku `productionplanitem`
--
ALTER TABLE `productionplanitem`
  ADD PRIMARY KEY (`PASTRY_PASTRYID`,`PLAN_PLANID`),
  ADD KEY `FK_ProductionPlanItem_PLAN_PLANID` (`PLAN_PLANID`);

--
-- Klíče pro tabulku `user`
--
ALTER TABLE `user`
  ADD PRIMARY KEY (`USERID`),
  ADD UNIQUE KEY `EMAIL` (`EMAIL`);

--
-- AUTO_INCREMENT pro tabulky
--

--
-- AUTO_INCREMENT pro tabulku `material`
--
ALTER TABLE `material`
  MODIFY `MATERIALID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=42;

--
-- AUTO_INCREMENT pro tabulku `materialorder`
--
ALTER TABLE `materialorder`
  MODIFY `ORDERID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=13;

--
-- AUTO_INCREMENT pro tabulku `pastry`
--
ALTER TABLE `pastry`
  MODIFY `PASTRYID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=24;

--
-- AUTO_INCREMENT pro tabulku `pastrycategory`
--
ALTER TABLE `pastrycategory`
  MODIFY `CATEGORYID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT pro tabulku `pastryorder`
--
ALTER TABLE `pastryorder`
  MODIFY `ORDERID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=18;

--
-- AUTO_INCREMENT pro tabulku `productionplan`
--
ALTER TABLE `productionplan`
  MODIFY `PLANID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=8;

--
-- AUTO_INCREMENT pro tabulku `user`
--
ALTER TABLE `user`
  MODIFY `USERID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=18;

--
-- Omezení pro exportované tabulky
--

--
-- Omezení pro tabulku `materialorderitem`
--
ALTER TABLE `materialorderitem`
  ADD CONSTRAINT `FK_MaterialOrderItem_MATERIAL_MATERIALID` FOREIGN KEY (`MATERIAL_MATERIALID`) REFERENCES `material` (`MATERIALID`),
  ADD CONSTRAINT `FK_MaterialOrderItem_ORDER_ORDERID` FOREIGN KEY (`ORDER_ORDERID`) REFERENCES `materialorder` (`ORDERID`);

--
-- Omezení pro tabulku `materialstorage`
--
ALTER TABLE `materialstorage`
  ADD CONSTRAINT `FK_MaterialStorage_MATERIAL_MATERIALID` FOREIGN KEY (`MATERIAL_MATERIALID`) REFERENCES `material` (`MATERIALID`);

--
-- Omezení pro tabulku `pastry`
--
ALTER TABLE `pastry`
  ADD CONSTRAINT `FK_Pastry_CATEGORY_CATEGORYID` FOREIGN KEY (`CATEGORY_CATEGORYID`) REFERENCES `pastrycategory` (`CATEGORYID`);

--
-- Omezení pro tabulku `pastrymaterial`
--
ALTER TABLE `pastrymaterial`
  ADD CONSTRAINT `FK_PastryMaterial_MATERIAL_MATERIALID` FOREIGN KEY (`MATERIAL_MATERIALID`) REFERENCES `material` (`MATERIALID`),
  ADD CONSTRAINT `FK_PastryMaterial_PASTRY_PASTRYID` FOREIGN KEY (`PASTRY_PASTRYID`) REFERENCES `pastry` (`PASTRYID`);

--
-- Omezení pro tabulku `pastryorder`
--
ALTER TABLE `pastryorder`
  ADD CONSTRAINT `FK_PastryOrder_CUSTOMER_USERID` FOREIGN KEY (`CUSTOMER_USERID`) REFERENCES `user` (`USERID`);

--
-- Omezení pro tabulku `pastryorderitem`
--
ALTER TABLE `pastryorderitem`
  ADD CONSTRAINT `FK_PastryOrderItem_ORDER_ORDERID` FOREIGN KEY (`ORDER_ORDERID`) REFERENCES `pastryorder` (`ORDERID`),
  ADD CONSTRAINT `FK_PastryOrderItem_PASTRY_PASTRYID` FOREIGN KEY (`PASTRY_PASTRYID`) REFERENCES `pastry` (`PASTRYID`);

--
-- Omezení pro tabulku `pastrystorage`
--
ALTER TABLE `pastrystorage`
  ADD CONSTRAINT `FK_PastryStorage_PASTRY_PASTRYID` FOREIGN KEY (`PASTRY_PASTRYID`) REFERENCES `pastry` (`PASTRYID`);

--
-- Omezení pro tabulku `productionplanitem`
--
ALTER TABLE `productionplanitem`
  ADD CONSTRAINT `FK_ProductionPlanItem_PASTRY_PASTRYID` FOREIGN KEY (`PASTRY_PASTRYID`) REFERENCES `pastry` (`PASTRYID`),
  ADD CONSTRAINT `FK_ProductionPlanItem_PLAN_PLANID` FOREIGN KEY (`PLAN_PLANID`) REFERENCES `productionplan` (`PLANID`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
