-- phpMyAdmin SQL Dump
-- version 3.4.5
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Nov 28, 2017 at 08:23 PM
-- Server version: 5.6.33
-- PHP Version: 5.3.29

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `xaubre02`
--

-- --------------------------------------------------------

--
-- Table structure for table `Ded_misto`
--

CREATE TABLE IF NOT EXISTS `Ded_misto` (
  `jmeno` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `druh` varchar(32) COLLATE latin2_czech_cs NOT NULL,
  `procento` int(11) NOT NULL,
  PRIMARY KEY (`jmeno`),
  KEY `druh` (`druh`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Dumping data for table `Ded_misto`
--

INSERT INTO `Ded_misto` (`jmeno`, `druh`, `procento`) VALUES
('Bradavice', 'vzduch', 64),
('Ereldin', 'země', 25),
('Mariánský příkop', 'voda', 99),
('Mount Doom', 'oheň', 77),
('Random place', 'vzduch', 18);

-- --------------------------------------------------------

--
-- Table structure for table `Element`
--

CREATE TABLE IF NOT EXISTS `Element` (
  `druh` varchar(32) COLLATE latin2_czech_cs NOT NULL,
  `specializace` varchar(32) CHARACTER SET utf8 COLLATE utf8_czech_ci NOT NULL,
  `barva` varchar(32) CHARACTER SET utf8 COLLATE utf8_czech_ci NOT NULL,
  PRIMARY KEY (`druh`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Dumping data for table `Element`
--

INSERT INTO `Element` (`druh`, `specializace`, `barva`) VALUES
('oheň', 'útok', 'červená'),
('voda', 'podpora', 'modrá'),
('vzduch', 'podpora', 'bílá'),
('země', 'obrana', 'hnědá');

-- --------------------------------------------------------

--
-- Table structure for table `Grimoar`
--

CREATE TABLE IF NOT EXISTS `Grimoar` (
  `nazev_grim` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `mana` int(11) NOT NULL,
  `druh` varchar(32) COLLATE latin2_czech_cs NOT NULL,
  `jmeno_kouz` varchar(100) COLLATE latin2_czech_cs DEFAULT NULL,
  PRIMARY KEY (`nazev_grim`),
  KEY `druh` (`druh`),
  KEY `jmeno_kouz` (`jmeno_kouz`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Dumping data for table `Grimoar`
--

INSERT INTO `Grimoar` (`nazev_grim`, `mana`, `druh`, `jmeno_kouz`) VALUES
('Kapesní encyklopedie', 18, 'voda', NULL),
('Kouzla od A do Z', 42360, 'vzduch', 'Lachtan'),
('Mocná knížka', 100, 'vzduch', 'Merlin'),
('Spell Book', 666, 'oheň', 'aubi'),
('Velká kniha čar a kouzel', 42000, 'voda', NULL);

-- --------------------------------------------------------

--
-- Table structure for table `Grim_obsah`
--

CREATE TABLE IF NOT EXISTS `Grim_obsah` (
  `nazev_kouz` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `nazev_grim` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  PRIMARY KEY (`nazev_kouz`,`nazev_grim`),
  KEY `nazev_grim` (`nazev_grim`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Dumping data for table `Grim_obsah`
--

INSERT INTO `Grim_obsah` (`nazev_kouz`, `nazev_grim`) VALUES
('Ice-block', 'Kapesní encyklopedie'),
('Mirror entity', 'Kouzla od A do Z'),
('Swipe', 'Kouzla od A do Z'),
('Ultimate Infestation', 'Kouzla od A do Z'),
('Droplets of purity', 'Mocná knížka'),
('Fireball', 'Mocná knížka'),
('Fire-block', 'Mocná knížka'),
('Fire-lance', 'Mocná knížka'),
('Ice-block', 'Mocná knížka'),
('Fire-block', 'Spell Book'),
('Droplets of purity', 'Velká kniha čar a kouzel'),
('Fireball', 'Velká kniha čar a kouzel'),
('Fire-lance', 'Velká kniha čar a kouzel');

-- --------------------------------------------------------

--
-- Table structure for table `Kouzelnik`
--

CREATE TABLE IF NOT EXISTS `Kouzelnik` (
  `jmeno_kouz` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `heslo` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `mana` int(11) NOT NULL DEFAULT '0',
  `uroven` varchar(2) COLLATE latin2_czech_cs NOT NULL DEFAULT 'E',
  `synergie` varchar(32) COLLATE latin2_czech_cs DEFAULT NULL,
  PRIMARY KEY (`jmeno_kouz`),
  KEY `synergie` (`synergie`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Dumping data for table `Kouzelnik`
--

INSERT INTO `Kouzelnik` (`jmeno_kouz`, `heslo`, `mana`, `uroven`, `synergie`) VALUES
('aubi', 'e10adc3949ba59abbe56e057f20f883e', 99999, 'SS', 'oheň'),
('Baba Yaga', 'e10adc3949ba59abbe56e057f20f883e', 1500, 'E', NULL),
('Gandalf', 'e10adc3949ba59abbe56e057f20f883e', 20000, 'B', NULL),
('Guldan', 'e10adc3949ba59abbe56e057f20f883e', 44, 'C', NULL),
('Jaina', 'e10adc3949ba59abbe56e057f20f883e', 78, 'A', 'země'),
('Lachtan', 'e10adc3949ba59abbe56e057f20f883e', 12, 'D', 'oheň'),
('Libek', 'e10adc3949ba59abbe56e057f20f883e', 2, 'E', 'vzduch'),
('Merlin', 'e10adc3949ba59abbe56e057f20f883e', 8900, 'C', 'země');

-- --------------------------------------------------------

--
-- Table structure for table `Kouzlo`
--

CREATE TABLE IF NOT EXISTS `Kouzlo` (
  `nazev_kouz` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `typ` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `obtiznost` varchar(10) COLLATE latin2_czech_cs NOT NULL,
  `sila` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `druh` varchar(32) COLLATE latin2_czech_cs NOT NULL,
  PRIMARY KEY (`nazev_kouz`),
  KEY `druh` (`druh`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Dumping data for table `Kouzlo`
--

INSERT INTO `Kouzlo` (`nazev_kouz`, `typ`, `obtiznost`, `sila`, `druh`) VALUES
('Droplets of purity', 'útočné', 'těžká', 'velmi silné', 'voda'),
('Fireball', 'útočné', 'těžká', 'silné', 'oheň'),
('Fire-block', 'obranné', 'střední', 'velmi slabé', 'oheň'),
('Fire-lance', 'útočné', 'střední', 'slabé', 'oheň'),
('Ice-block', 'obranné', 'střední', 'silné', 'voda'),
('Mirror entity', 'obranné', 'snadná', 'slabé', 'země'),
('Mozkocuc', 'útočné', 'snadná', 'velmi slabé', 'vzduch'),
('Swipe', 'útočné', 'střední', 'silné', 'vzduch'),
('Ultimate Infestation', 'útočné', 'těžká', 'velmi silné', 'země');

-- --------------------------------------------------------

--
-- Table structure for table `Kouz_mel`
--

CREATE TABLE IF NOT EXISTS `Kouz_mel` (
  `jmeno_kouz` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `nazev_grim` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `mel_od` date NOT NULL,
  `mel_do` date NOT NULL,
  PRIMARY KEY (`jmeno_kouz`,`nazev_grim`,`mel_od`,`mel_do`),
  KEY `fk_kz_mel_g` (`nazev_grim`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Dumping data for table `Kouz_mel`
--

INSERT INTO `Kouz_mel` (`jmeno_kouz`, `nazev_grim`, `mel_od`, `mel_do`) VALUES
('Baba Yaga', 'Kapesní encyklopedie', '2017-11-09', '2018-11-16'),
('Lachtan', 'Kapesní encyklopedie', '2014-11-02', '2015-06-15'),
('Baba Yaga', 'Velká kniha čar a kouzel', '1025-06-21', '1142-04-13'),
('Gandalf', 'Velká kniha čar a kouzel', '1215-01-01', '1215-01-01'),
('Gandalf', 'Velká kniha čar a kouzel', '1215-01-31', '1217-03-08'),
('Guldan', 'Velká kniha čar a kouzel', '1945-11-28', '2017-11-28'),
('Merlin', 'Velká kniha čar a kouzel', '1215-01-01', '1215-01-31');

-- --------------------------------------------------------

--
-- Table structure for table `Kouz_umi`
--

CREATE TABLE IF NOT EXISTS `Kouz_umi` (
  `jmeno_kouz` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `nazev_kouz` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  PRIMARY KEY (`jmeno_kouz`,`nazev_kouz`),
  KEY `nazev_kouz` (`nazev_kouz`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Dumping data for table `Kouz_umi`
--

INSERT INTO `Kouz_umi` (`jmeno_kouz`, `nazev_kouz`) VALUES
('Baba Yaga', 'Fireball'),
('Gandalf', 'Fireball'),
('Gandalf', 'Fire-block'),
('Libek', 'Mirror entity'),
('Merlin', 'Swipe'),
('aubi', 'Ultimate Infestation');

-- --------------------------------------------------------

--
-- Table structure for table `Ma_ved_elem`
--

CREATE TABLE IF NOT EXISTS `Ma_ved_elem` (
  `nazev_kouz` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `druh` varchar(32) COLLATE latin2_czech_cs NOT NULL,
  PRIMARY KEY (`nazev_kouz`,`druh`),
  KEY `druh` (`druh`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Dumping data for table `Ma_ved_elem`
--

INSERT INTO `Ma_ved_elem` (`nazev_kouz`, `druh`) VALUES
('Fire-block', 'voda'),
('Mirror entity', 'voda'),
('Ice-block', 'vzduch'),
('Mirror entity', 'vzduch');

-- --------------------------------------------------------

--
-- Table structure for table `Nabiti`
--

CREATE TABLE IF NOT EXISTS `Nabiti` (
  `id_nabiti` int(11) NOT NULL AUTO_INCREMENT,
  `datum` date NOT NULL,
  `nazev_grim` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `jmeno_kouz` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  PRIMARY KEY (`id_nabiti`),
  KEY `nazev_grim` (`nazev_grim`),
  KEY `jmeno_kouz` (`jmeno_kouz`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs AUTO_INCREMENT=6 ;

--
-- Dumping data for table `Nabiti`
--

INSERT INTO `Nabiti` (`id_nabiti`, `datum`, `nazev_grim`, `jmeno_kouz`) VALUES
(1, '1520-03-18', 'Mocná knížka', 'Guldan'),
(3, '1666-07-16', 'Kapesní encyklopedie', 'Baba Yaga'),
(4, '1315-03-22', 'Kouzla od A do Z', 'Lachtan'),
(5, '1749-12-18', 'Spell Book', 'Jaina');

-- --------------------------------------------------------

--
-- Table structure for table `Nab_ded_mista`
--

CREATE TABLE IF NOT EXISTS `Nab_ded_mista` (
  `Nabiti` int(11) NOT NULL,
  `misto` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  PRIMARY KEY (`Nabiti`,`misto`),
  KEY `misto` (`misto`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Dumping data for table `Nab_ded_mista`
--

INSERT INTO `Nab_ded_mista` (`Nabiti`, `misto`) VALUES
(3, 'Bradavice'),
(4, 'Mariánský příkop'),
(1, 'Mount Doom'),
(5, 'Random place');

-- --------------------------------------------------------

--
-- Table structure for table `Svitek`
--

CREATE TABLE IF NOT EXISTS `Svitek` (
  `id_svitku` int(11) NOT NULL AUTO_INCREMENT,
  `nazev` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `nazev_kouz` varchar(100) COLLATE latin2_czech_cs NOT NULL,
  `jmeno_kouz` varchar(100) COLLATE latin2_czech_cs DEFAULT NULL,
  PRIMARY KEY (`id_svitku`),
  KEY `nazev_kouz` (`nazev_kouz`),
  KEY `jmeno_kouz` (`jmeno_kouz`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs AUTO_INCREMENT=8 ;

--
-- Dumping data for table `Svitek`
--

INSERT INTO `Svitek` (`id_svitku`, `nazev`, `nazev_kouz`, `jmeno_kouz`) VALUES
(1, 'Svitek pánů z Mordoru', 'Fire-lance', 'Gandalf'),
(2, 'Elder Scroll', 'Fireball', NULL),
(7, 'Libkův svitek demence', 'Mozkocuc', 'Libek');

--
-- Constraints for dumped tables
--

--
-- Constraints for table `Ded_misto`
--
ALTER TABLE `Ded_misto`
  ADD CONSTRAINT `Ded_misto_ibfk_1` FOREIGN KEY (`druh`) REFERENCES `Element` (`druh`) ON UPDATE CASCADE;

--
-- Constraints for table `Grimoar`
--
ALTER TABLE `Grimoar`
  ADD CONSTRAINT `Grimoar_ibfk_1` FOREIGN KEY (`druh`) REFERENCES `Element` (`druh`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Grimoar_ibfk_2` FOREIGN KEY (`jmeno_kouz`) REFERENCES `Kouzelnik` (`jmeno_kouz`) ON UPDATE CASCADE;

--
-- Constraints for table `Grim_obsah`
--
ALTER TABLE `Grim_obsah`
  ADD CONSTRAINT `Grim_obsah_ibfk_1` FOREIGN KEY (`nazev_kouz`) REFERENCES `Kouzlo` (`nazev_kouz`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Grim_obsah_ibfk_2` FOREIGN KEY (`nazev_grim`) REFERENCES `Grimoar` (`nazev_grim`) ON UPDATE CASCADE;

--
-- Constraints for table `Kouzelnik`
--
ALTER TABLE `Kouzelnik`
  ADD CONSTRAINT `Kouzelnik_ibfk_1` FOREIGN KEY (`synergie`) REFERENCES `Element` (`druh`) ON UPDATE CASCADE;

--
-- Constraints for table `Kouzlo`
--
ALTER TABLE `Kouzlo`
  ADD CONSTRAINT `Kouzlo_ibfk_1` FOREIGN KEY (`druh`) REFERENCES `Element` (`druh`) ON UPDATE CASCADE;

--
-- Constraints for table `Kouz_mel`
--
ALTER TABLE `Kouz_mel`
  ADD CONSTRAINT `fk_kz_mel_g` FOREIGN KEY (`nazev_grim`) REFERENCES `Grimoar` (`nazev_grim`),
  ADD CONSTRAINT `fk_kz_mel_k` FOREIGN KEY (`jmeno_kouz`) REFERENCES `Kouzelnik` (`jmeno_kouz`);

--
-- Constraints for table `Kouz_umi`
--
ALTER TABLE `Kouz_umi`
  ADD CONSTRAINT `Kouz_umi_ibfk_1` FOREIGN KEY (`jmeno_kouz`) REFERENCES `Kouzelnik` (`jmeno_kouz`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Kouz_umi_ibfk_2` FOREIGN KEY (`nazev_kouz`) REFERENCES `Kouzlo` (`nazev_kouz`) ON UPDATE CASCADE;

--
-- Constraints for table `Ma_ved_elem`
--
ALTER TABLE `Ma_ved_elem`
  ADD CONSTRAINT `Ma_ved_elem_ibfk_1` FOREIGN KEY (`nazev_kouz`) REFERENCES `Kouzlo` (`nazev_kouz`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Ma_ved_elem_ibfk_2` FOREIGN KEY (`druh`) REFERENCES `Element` (`druh`) ON UPDATE CASCADE;

--
-- Constraints for table `Nabiti`
--
ALTER TABLE `Nabiti`
  ADD CONSTRAINT `Nabiti_ibfk_5` FOREIGN KEY (`nazev_grim`) REFERENCES `Grimoar` (`nazev_grim`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `Nabiti_ibfk_6` FOREIGN KEY (`jmeno_kouz`) REFERENCES `Kouzelnik` (`jmeno_kouz`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `Nab_ded_mista`
--
ALTER TABLE `Nab_ded_mista`
  ADD CONSTRAINT `Nab_ded_mista_ibfk_1` FOREIGN KEY (`Nabiti`) REFERENCES `Nabiti` (`id_nabiti`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Nab_ded_mista_ibfk_2` FOREIGN KEY (`misto`) REFERENCES `Ded_misto` (`jmeno`) ON UPDATE CASCADE;

--
-- Constraints for table `Svitek`
--
ALTER TABLE `Svitek`
  ADD CONSTRAINT `Svitek_ibfk_1` FOREIGN KEY (`nazev_kouz`) REFERENCES `Kouzlo` (`nazev_kouz`) ON UPDATE CASCADE,
  ADD CONSTRAINT `Svitek_ibfk_2` FOREIGN KEY (`jmeno_kouz`) REFERENCES `Kouzelnik` (`jmeno_kouz`) ON UPDATE CASCADE;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
