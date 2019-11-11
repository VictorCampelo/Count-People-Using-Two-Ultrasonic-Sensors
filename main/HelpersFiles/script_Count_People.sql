-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

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
  `idnode` INT NOT NULL,
  PRIMARY KEY (`idnode`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `iot_count_people`.`countReg`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `iot_count_people`.`countReg` ;

CREATE TABLE IF NOT EXISTS `iot_count_people`.`countReg` (
  `idcountReg` INT NOT NULL AUTO_INCREMENT,
  `numPeople` INT NOT NULL,
  `dtime` DATETIME NOT NULL,
  `node_idnode` INT NOT NULL,
  PRIMARY KEY (`idcountReg`),
  INDEX `fk_countReg_node_idx` (`node_idnode` ASC),
  CONSTRAINT `fk_countReg_node`
    FOREIGN KEY (`node_idnode`)
    REFERENCES `iot_count_people`.`node` (`idnode`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
