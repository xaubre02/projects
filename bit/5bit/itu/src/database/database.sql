-- phpMyAdmin SQL Dump
-- version 3.4.5
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Dec 04, 2017 at 01:22 PM
-- Server version: 5.6.33
-- PHP Version: 5.3.29

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `xambro15`
--

-- --------------------------------------------------------

--
-- Table structure for table `Akce`
--

CREATE TABLE IF NOT EXISTS `Akce` (
  `cislo_zbozi` int(11) NOT NULL,
  `akcni_cena` int(11) NOT NULL,
  KEY `FK_cislo_zbozi` (`cislo_zbozi`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Dumping data for table `Akce`
--

INSERT INTO `Akce` (`cislo_zbozi`, `akcni_cena`) VALUES
(1, 350),
(8, 200),
(9, 150),
(24, 50),
(19, 65);

-- --------------------------------------------------------

--
-- Table structure for table `Kategorie`
--

CREATE TABLE IF NOT EXISTS `Kategorie` (
  `nazevKategorie` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  `hlavniKategorie` varchar(30) COLLATE utf8_czech_ci DEFAULT NULL,
  PRIMARY KEY (`nazevKategorie`),
  KEY `hlavniKategorie` (`hlavniKategorie`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Dumping data for table `Kategorie`
--

INSERT INTO `Kategorie` (`nazevKategorie`, `hlavniKategorie`) VALUES
('Celoroční sporty', NULL),
('Kolektivní sporty', NULL),
('Letní sporty', NULL),
('Zimní sporty', NULL),
('Horská kola', 'Celoroční sporty'),
('Silniční kola', 'Celoroční sporty'),
('Tenis', 'Celoroční sporty'),
('Florbal', 'Kolektivní sporty'),
('Fotbal', 'Kolektivní sporty'),
('Volejbal', 'Kolektivní sporty'),
('Paddleboardy', 'Letní sporty'),
('Surfy', 'Letní sporty'),
('Brusle', 'Zimní sporty'),
('Lyže', 'Zimní sporty'),
('Snowboardy', 'Zimní sporty');

-- --------------------------------------------------------

--
-- Table structure for table `KosikObsahuje`
--

CREATE TABLE IF NOT EXISTS `KosikObsahuje` (
  `email` varchar(40) COLLATE utf8_czech_ci NOT NULL,
  `cislo_zbozi` int(11) NOT NULL,
  `datumOd` date NOT NULL,
  `datumDo` date NOT NULL,
  `cenaDen` int(11) NOT NULL,
  KEY `cislo_zbozi` (`cislo_zbozi`),
  KEY `email` (`email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Dumping data for table `KosikObsahuje`
--

INSERT INTO `KosikObsahuje` (`email`, `cislo_zbozi`, `datumOd`, `datumDo`, `cenaDen`) VALUES
('vitekv99@seznam.cz', 2, '2017-11-29', '2017-11-30', 600);

-- --------------------------------------------------------

--
-- Table structure for table `Rezervuje`
--

CREATE TABLE IF NOT EXISTS `Rezervuje` (
  `email` varchar(40) COLLATE utf8_czech_ci NOT NULL,
  `cislo_zbozi` int(11) NOT NULL,
  `datumOd` date NOT NULL,
  `datumDo` date NOT NULL,
  `cenaDen` int(11) NOT NULL,
  KEY `cislo_zbozi` (`cislo_zbozi`),
  KEY `FK_email` (`email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Dumping data for table `Rezervuje`
--

INSERT INTO `Rezervuje` (`email`, `cislo_zbozi`, `datumOd`, `datumDo`, `cenaDen`) VALUES
('dabednarik@gmail.com', 1, '2017-11-20', '2017-11-20', 250),
('dabednarik@gmail.com', 2, '2017-11-20', '2017-11-22', 450),
('dabednarik@gmail.com', 1, '2017-11-24', '2017-11-26', 500),
('vitekv99@seznam.cz', 1, '2017-11-27', '2017-11-28', 350),
('admin@gmail.com', 1, '2017-11-29', '2017-11-30', 350),
('admin@gmail.com', 3, '2017-12-13', '2017-12-15', 250),
('admin@gmail.com', 9, '2017-12-19', '2017-12-24', 150),
('admin@gmail.com', 18, '2017-12-03', '2017-12-14', 100),
('admin@gmail.com', 1, '2017-12-24', '2017-12-29', 350);

-- --------------------------------------------------------

--
-- Table structure for table `Uzivatel`
--

CREATE TABLE IF NOT EXISTS `Uzivatel` (
  `heslo` char(32) COLLATE utf8_czech_ci NOT NULL,
  `jmeno` varchar(10) COLLATE utf8_czech_ci NOT NULL,
  `prijmeni` varchar(10) COLLATE utf8_czech_ci NOT NULL,
  `telefon` varchar(9) COLLATE utf8_czech_ci NOT NULL,
  `email` varchar(40) COLLATE utf8_czech_ci NOT NULL,
  `ulice` varchar(20) COLLATE utf8_czech_ci DEFAULT NULL,
  `mesto` varchar(20) COLLATE utf8_czech_ci DEFAULT NULL,
  `psc` int(11) DEFAULT NULL,
  PRIMARY KEY (`email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Dumping data for table `Uzivatel`
--

INSERT INTO `Uzivatel` (`heslo`, `jmeno`, `prijmeni`, `telefon`, `email`, `ulice`, `mesto`, `psc`) VALUES
('25f9e794323b453885f5181f1b624d0b', 'Mickey', 'Mouse', '123456789', 'admin@gmail.com', 'Walt', 'Disney', 12345),
('692e4b4d68e2af1fe32c3c996fe4aa54', 'David', 'Bednařík', '777166399', 'dabednarik@gmail.com', 'U Pískovny 341/6', 'Ostrava', 72527),
('25f9e794323b453885f5181f1b624d0b', 'Vít', 'Ambrož', '723379494', 'Vitekv99@seznam.cz', 'Horova 51', 'Brno', 11111);

-- --------------------------------------------------------

--
-- Table structure for table `Zbozi`
--

CREATE TABLE IF NOT EXISTS `Zbozi` (
  `cislo_zbozi` int(11) NOT NULL AUTO_INCREMENT,
  `nazev` varchar(20) COLLATE utf8_czech_ci NOT NULL,
  `cena` int(11) NOT NULL,
  `zaloha` int(11) NOT NULL,
  `datumVyroby` date NOT NULL,
  `popis` varchar(60) COLLATE utf8_czech_ci DEFAULT NULL,
  `obrazek` varchar(40) COLLATE utf8_czech_ci DEFAULT NULL,
  `nazevKategorie` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  PRIMARY KEY (`cislo_zbozi`),
  KEY `nazevKategorie` (`nazevKategorie`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci AUTO_INCREMENT=36 ;

--
-- Dumping data for table `Zbozi`
--

INSERT INTO `Zbozi` (`cislo_zbozi`, `nazev`, `cena`, `zaloha`, `datumVyroby`, `popis`, `obrazek`, `nazevKategorie`) VALUES
(1, 'Černé lyže', 500, 2000, '2011-02-10', 'Jako nové.', 'black_ski.jpg', 'Lyže'),
(2, 'Modré lyže', 600, 2500, '2008-11-13', 'Ve velmi dobrém stavu.', 'blue_ski.jpg', 'Lyže'),
(3, 'Specilized Ghost', 250, 5000, '2015-12-15', 'Velmi lehké horské kolo s odpruženou přední vidlicí.', 'specilized_bike1.jpg', 'Horská kola'),
(4, 'Specilized Raptor', 400, 8000, '2016-01-12', 'Ultra lehké celo odpružené horské kolo.', 'specilized_bike2.jpg', 'Horská kola'),
(5, 'Specilized Fatboy', 600, 8000, '2015-12-12', 'Horské kolo s tlustými plášti.', 'specilized_bike3.jpg', 'Horská kola'),
(6, 'S-WORKS', 800, 8000, '2015-12-12', 'Ultra lehké silniční kolo.', 'specilized_bike4.jpg', 'Silniční kola'),
(7, 'Author Faster', 350, 5000, '2010-02-03', 'Komfortní silniční kolo.', 'specilized_bike5.jpg', 'Silniční kola'),
(8, 'Favorit S1-SPORT', 300, 3000, '1999-05-06', 'Velmi zachované silniční kolo s úspěšnou historii.', 'specilized_bike6.jpg', 'Silniční kola'),
(9, 'Lifetime Amped', 250, 2500, '2010-04-03', 'Nafukovací paddleboard ve výborném stavu.', 'paddleboard1.jpg', 'Paddleboardy'),
(10, 'Slide BIC', 400, 3500, '2016-09-05', 'Velmi odolný paddleboard. Není nafukovací.', 'paddleboard2.jpg', 'Paddleboardy'),
(11, 'Timbertek', 500, 4000, '2015-02-19', 'Dřevěný surf.', 'surf1.jpg', 'Surfy'),
(12, 'SPAM', 600, 4500, '2014-07-16', 'Ultra lehký surf vyrobený z uhlákových vláken.', 'surf2.jpeg', 'Surfy'),
(13, '10x rozlišovací dres', 50, 500, '2017-11-01', 'Sada 10 tréninkových rozlišovacích dresů Nike (tyrkysová)', 'rozlisovak_nike.jpg', 'Fotbal'),
(14, '10x rozlišovací dres', 60, 600, '2017-11-01', 'Sada 10 rozlišovacích dresů Adidas (oranžová)', 'rozlisovak_adidas.jpg', 'Fotbal'),
(15, 'Míč Adidas L. Strike', 100, 300, '2017-11-01', 'Fotbalový míč Adidas League Strike', 'mic-adidas-league-strike.jpg', 'Fotbal'),
(16, 'Míč Adidas Glider', 80, 250, '2017-11-01', 'Fotbalový míč Adidas Top Glider (Euro 2016)', 'mic-adidas-top-glider.jpg', 'Fotbal'),
(17, 'Dětská hůl Kempish', 60, 350, '2017-11-01', 'Dětská florbalová hůl Kempish (délka - 120cm)', 'detska-florbal-kensis.jpg', 'Florbal'),
(18, 'Hůl Wooloc', 100, 600, '2017-11-01', 'Florbalová hůl Wooloc (délka - 160cm)', 'florbal-Wooloc.jpg', 'Florbal'),
(19, 'Maska Tempish', 90, 700, '2017-11-01', 'Florbalová maska Tempish (oranžová)', 'maska-tempish.jpg', 'Florbal'),
(20, 'Maska Salming', 100, 700, '2017-11-01', 'Florbalová maska Salming (modrá, černá)', 'Maska-salming.jpg', 'Florbal'),
(21, 'Míč GALA', 60, 250, '2017-11-01', 'Volejbalový míč GALA Training-10', 'mic-gala-training-10.jpg', 'Volejbal'),
(22, 'Míč Wilson', 40, 200, '2017-11-01', 'Volejbalový míč Wilson QuickSand', 'mic-wilson.jpg', 'Volejbal'),
(23, 'Volejbalová síť', 80, 500, '2017-11-01', 'Volejbalová síť MTB-group', 'sit-mtb-group.jpg', 'Volejbal'),
(24, 'Dětská raketa Wilson', 80, 500, '2017-11-01', 'Dětská tenisová raketa Wilson Blade Team', 'detska-tenisova-raketa-wilson-blade-team', 'Tenis'),
(25, 'Raketa Dunlop', 120, 750, '2017-11-01', 'Tenisová raketa Dunlop-Predator 95', 'tenisova-raketa-dunlop-predator-95.jpg', 'Tenis'),
(26, 'Raketa Wilson', 100, 700, '2017-11-01', 'Tenisová raketa Wilson Burn 100', 'tenisova-raketa-wilson-burn-100.jpg', 'Tenis'),
(27, 'Brusle Tempish', 120, 650, '2016-09-15', 'Elegantní brusle ve stylu Converse.', 'brusle0.jpg', 'Brusle'),
(28, 'Brusle Dagmar', 200, 799, '2017-01-10', 'Elita mezi krasobruslemi s ocelové ostřím.', 'brusle1.jpg', 'Brusle'),
(29, 'Rollerblade Spark', 130, 650, '2015-04-09', 'Ideální pro bruslení na kluzišti, nebo zamrzlém rybníku.', 'roller.jpg', 'Brusle'),
(30, 'Rebook 1K', 180, 800, '2016-06-09', 'Určeny pro hobby hráče ledního hokeje.', 'rebook.jpg', 'Brusle'),
(31, 'Elan Amphibio', 600, 2300, '2014-06-13', 'Odlišná konstrukce pravé a levé lyže.', 'lyze.jpg', 'Lyže'),
(32, 'Head Shape', 500, 2200, '2016-12-14', 'Rekreačně sportovní univerzální carvingová lyže.', 'lyze1.jpg', 'Lyže'),
(33, 'Morrow Blaze', 700, 3700, '2016-08-01', 'Dětský snowboard s ocelovým okrajem.', 'snowboard_bl_ye.jpg', 'Snowboardy'),
(34, 'Morrow Mountain', 800, 4300, '2016-08-01', 'Pánský snowboard je určen především na all-mountain.', 'snowboard_blue.jpg', 'Snowboardy'),
(35, 'Morrow Truth', 950, 4900, '2016-08-01', 'Pánský snowboard určen především na freestyle.', 'snowboard_black.jpg', 'Snowboardy');

--
-- Constraints for dumped tables
--

--
-- Constraints for table `Akce`
--
ALTER TABLE `Akce`
  ADD CONSTRAINT `FK_cislo_zbozi` FOREIGN KEY (`cislo_zbozi`) REFERENCES `Zbozi` (`cislo_zbozi`) ON DELETE CASCADE;

--
-- Constraints for table `Kategorie`
--
ALTER TABLE `Kategorie`
  ADD CONSTRAINT `Kategorie_ibfk_1` FOREIGN KEY (`hlavniKategorie`) REFERENCES `Kategorie` (`nazevKategorie`) ON DELETE CASCADE;

--
-- Constraints for table `KosikObsahuje`
--
ALTER TABLE `KosikObsahuje`
  ADD CONSTRAINT `KosikObsahuje_ibfk_1` FOREIGN KEY (`email`) REFERENCES `Uzivatel` (`email`) ON DELETE CASCADE,
  ADD CONSTRAINT `KosikObsahuje_ibfk_2` FOREIGN KEY (`cislo_zbozi`) REFERENCES `Zbozi` (`cislo_zbozi`) ON DELETE CASCADE;

--
-- Constraints for table `Rezervuje`
--
ALTER TABLE `Rezervuje`
  ADD CONSTRAINT `Rezervuje_ibfk_1` FOREIGN KEY (`email`) REFERENCES `Uzivatel` (`email`) ON DELETE CASCADE,
  ADD CONSTRAINT `Rezervuje_ibfk_2` FOREIGN KEY (`cislo_zbozi`) REFERENCES `Zbozi` (`cislo_zbozi`) ON DELETE CASCADE;

--
-- Constraints for table `Zbozi`
--
ALTER TABLE `Zbozi`
  ADD CONSTRAINT `Zbozi_ibfk_1` FOREIGN KEY (`nazevKategorie`) REFERENCES `Kategorie` (`nazevKategorie`) ON DELETE CASCADE;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
