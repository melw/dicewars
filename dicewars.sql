-- MySQL dump 10.10
--
-- Host: localhost    Database: dicewars
-- ------------------------------------------------------
-- Server version	5.0.27

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `game`
--

DROP TABLE IF EXISTS `game`;
CREATE TABLE `game` (
  `gameId` int(10) NOT NULL auto_increment,
  `gameStarted` int(10) NOT NULL,
  `gameEnded` int(10) NOT NULL,
  PRIMARY KEY  (`gameId`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


--
-- Table structure for table `player`
--

DROP TABLE IF EXISTS `player`;
CREATE TABLE `player` (
  `playerId` int(10) NOT NULL auto_increment,
  `name` varchar(16) NOT NULL,
  `MAC` char(12) NOT NULL,
  `playerType` int(4) NOT NULL,
  `score` int(8) NOT NULL,
  `lastConnected` int(10) NOT NULL,
  PRIMARY KEY  (`playerId`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


--
-- Table structure for table `results`
--

DROP TABLE IF EXISTS `results`;
CREATE TABLE `results` (
  `gameId` int(10) NOT NULL,
  `playerId` int(10) NOT NULL,
  `position` int(4) NOT NULL,
  `score` int(8) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2008-08-16 17:11:26
