<?php 
?>
<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8">
        <title>Puppet Plant ｜ Demo</title>
        <link rel="stylesheet" type="text/css" href="css/stylesheet.css">  

        <!-- navigation -->
    <nav id="navbar">
    <img src="img/logo.png" alt="logo" class="logo">
    <br>
    <br>
    <ul class="nav-list">
        <li class="nav-option"><a href="#about">about</a></li>
        <li class="nav-option"><a href="#how-it-work">how it works</a></li>
        <li class="nav-option"><a href="#moving-forward">moving forward</a></li>
        <li class="nav-option"><a href="#the-makers">the makers</a></li>
        <li class="nav-option"><a href="demo.php">demo</a></li>
    </ul>
</nav>
<!-- end navigation -->  
    </head>

    <body>
    <?php
        $tempdata = $_POST['out_temp'];
        echo "<p class='text'>$tempdata</p>";
    ?>
    </body>
</html>
<?php
?>
