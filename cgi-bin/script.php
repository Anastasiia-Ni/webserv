<?php

$array;
if ($_SERVER['REQUEST_METHOD'] == 'GET')
{
    // echo "<h2>PHP is Fun!</h2>";
    parse_str($_SERVER['QUERY_STRING'], $array);
    echo "<h3>The list of environment variables :</h3>";
    foreach ($_SERVER as $key=>$value)
    {
        if (is_array($value))
            foreach ($value as $_key=>$_value)
                echo "$key [$_key] => $_value <br />";
        else
            echo "$key => $value <br />";
        
    }
}
else
{
    while( $line = fgets(STDIN) ) {
        parse_str($line, $array);
    }   
}

?>