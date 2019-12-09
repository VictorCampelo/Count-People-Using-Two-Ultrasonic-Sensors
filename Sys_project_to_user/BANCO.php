<?php



//$link = mysqli_connect("localhost", "root", "samuel123", "iot_cout_people");
$link = mysqli_connect("localhost", "root","", "iot_count_people");

if (!$link) {
	echo "Error: Falha ao conectar-se com o banco de dados MySQL." . PHP_EOL;
	echo "Debugging errno: " . mysqli_connect_errno() . PHP_EOL;
	echo "Debugging error: " . mysqli_connect_error() . PHP_EOL;
	exit;
}


$valor1 = 0 ;
$valor2 = "nery";


//$sql = "insert into site (contador) VALUES ($valor1)";

//mysqli_query($link,$sql);





//$sql2 = "select contador FROM site";

//SELECT countreg FROM site order by id DESC limit 1

$sql1 = "SELECT numPeople AS TOTAL FROM countreg order by idcountReg DESC limit 1";

$resultado = mysqli_query($link,$sql1);

$rs = mysqli_fetch_array($resultado);

echo $rs['TOTAL']."<br>"."<br>";



mysqli_close($link);

?>