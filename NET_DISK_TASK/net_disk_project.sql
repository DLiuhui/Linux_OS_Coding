-- MySQL dump 10.13  Distrib 5.7.22, for Linux (x86_64)
--
-- Host: localhost    Database: net_disk_project
-- ------------------------------------------------------
-- Server version	5.7.22-0ubuntu0.16.04.1

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
-- Table structure for table `FILE`
--

DROP TABLE IF EXISTS `FILE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `FILE` (
  `dir_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) NOT NULL,
  `file_name` varchar(256) DEFAULT NULL,
  `md5` varchar(32) DEFAULT NULL,
  `file_type` char(1) NOT NULL,
  `pre_dir_id` int(11) DEFAULT NULL,
  `file_size` bigint(20) unsigned DEFAULT NULL,
  PRIMARY KEY (`dir_id`),
  KEY `user_name` (`name`),
  CONSTRAINT `user_name` FOREIGN KEY (`name`) REFERENCES `USER` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `FILE`
--

LOCK TABLES `FILE` WRITE;
/*!40000 ALTER TABLE `FILE` DISABLE KEYS */;
INSERT INTO `FILE` VALUES (0,'root','root',NULL,'d',0,NULL),(1,'ddrh','ddrh',NULL,'d',0,NULL),(4,'ddrh','test',NULL,'d',1,NULL),(5,'1','1',NULL,'d',0,NULL),(8,'ddrh','数据库系统原理','8ca7408fedb615878a0f226a045a2888','f',1,170979718),(11,'1','数据库系统原理','8ca7408fedb615878a0f226a045a2888','f',5,170979718);
/*!40000 ALTER TABLE `FILE` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `USER`
--

DROP TABLE IF EXISTS `USER`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `USER` (
  `name` varchar(20) NOT NULL,
  `salt_code` varchar(11) NOT NULL,
  `code` varchar(98) DEFAULT NULL,
  `path` varchar(512) DEFAULT NULL,
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `USER`
--

LOCK TABLES `USER` WRITE;
/*!40000 ALTER TABLE `USER` DISABLE KEYS */;
INSERT INTO `USER` VALUES ('1','$6$yVvIT3s2','$6$yVvIT3s2$GqWrjOMpcaf9apkVQVQL81MjLftfv4ibLUiLefl8APNEmA11DKZJuyxDssm9E3VRxuJM0sH2ZrxIIJqv1tZke1','/root/1'),('ddrh','$6$b09AN6xn','$6$b09AN6xn$kyKwGC5BEAY5COnW0mTHa.LCvlAgHdAZgEA16.jtiDdMM1ndbA5T4EBjYjMiE9SkDYJTQARJMtdN6o9Y7eJ5C0','/root/ddrh'),('root','$6$pgYifhtI','$6$pgYifhtI$8ZAPzlwwbr5AbCRQQTfGGlZLA8ad9GbHZltJXWZM9jakTsN6a7Cp77hNUuiLeAQjb4ZsPDXw9ouXz2krnP5Fk/','/root');
/*!40000 ALTER TABLE `USER` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2018-07-20 14:48:53
