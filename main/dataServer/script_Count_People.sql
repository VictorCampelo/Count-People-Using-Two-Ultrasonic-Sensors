-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Schema mydb
-- -----------------------------------------------------
-- -----------------------------------------------------
-- Schema iot_count_people
-- -----------------------------------------------------
DROP SCHEMA IF EXISTS `iot_count_people` ;

-- -----------------------------------------------------
-- Schema iot_count_people
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `iot_count_people` DEFAULT CHARACTER SET utf8 ;
USE `iot_count_people` ;

-- -----------------------------------------------------
-- Table `iot_count_people`.`node`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `iot_count_people`.`node` ;

CREATE TABLE IF NOT EXISTS `iot_count_people`.`node` (
  `idnode` INT(11) NOT NULL,
  PRIMARY KEY (`idnode`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `iot_count_people`.`countreg`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `iot_count_people`.`countreg` ;

CREATE TABLE IF NOT EXISTS `iot_count_people`.`countreg` (
  `idcountReg` INT(11) NOT NULL AUTO_INCREMENT,
  `numPeople` INT(11) NOT NULL,
  `dtime` VARCHAR(30) NOT NULL,
  `node_idnode` INT(11) NOT NULL,
  PRIMARY KEY (`idcountReg`),
  INDEX `fk_countReg_node_idx` (`node_idnode` ASC) VISIBLE,
  CONSTRAINT `fk_countReg_node`
    FOREIGN KEY (`node_idnode`)
    REFERENCES `iot_count_people`.`node` (`idnode`))
ENGINE = InnoDB
AUTO_INCREMENT = 57
DEFAULT CHARACTER SET = utf8;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
