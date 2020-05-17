<template>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="stylesheet" 
    href="https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css" 
    integrity="sha384-Vkoo8x4CGsO3+Hhxv8T/Q5PaXtkKtu6ug5TOeNV6gBiFeWPGFN9MuhOf23Q9Ifjh"
    crossorigin="anonymous">
    <!-- ------------------------------------------------ -->
        <!-- Latest compiled and minified CSS -->
    <link rel="stylesheet" 
    href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" 
    integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u"
    crossorigin="anonymous">


      <!-- Optional theme -->
      <link 
      rel="stylesheet" 
      href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css"
      integrity="sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp" 
      crossorigin="anonymous">
    <link rel="stylesheet" href="/HTML/style.css"/>


    <title>IOT_PROJECT</title>
</head>
<body>
    <div id="app">
    <nav class="navbar navbar-expand-sm bg-dark navbar-dark nav justify-content-center">
        <!-- Brand -->
        <a class="navbar-brand" href="#"> <i class="fas fa-home"></i> Accueil</a>

        
        <!-- Links -->
        <ul class="navbar-nav">
          <li class="nav-item">
            <a class="nav-link" href="#sec4"><i class="fas fa-exclamation-triangle"></i> Alerte</a>
          </li>
        </ul>
      
        <!-- Links -->
        <ul class="navbar-nav">
          <li class="nav-item">
            <a class="nav-link" href="#sec2"><i class="fas fa-chart-line"></i> Dashbord</a>
          </li>
              <b-dropdown id="dropdown-1" text="Liste des salles" class="m-md-2">
                <b-dropdown-item><a href="#">C3</a></b-dropdown-item>
                    <b-dropdown-item><a href="#">C4</a></b-dropdown-item>
                    <b-dropdown-item><a href="#">C5</a></b-dropdown-item>
                    <b-dropdown-item><a href="#">TD12</a></b-dropdown-item>
                    <b-dropdown-item><a href="#">TD13</a></b-dropdown-item>
                    <b-dropdown-item><a href="#">TD14</a></b-dropdown-item>
                </b-dropdown>
        </ul>
        
          <form class="form-inline nav justify-content-right">
            <input class="form-control mr-sm-2" type="search" placeholder="Search" aria-label="Search">
            <button class="btn btn-outline-success my-2 my-sm-0" type="submit"> <i class="fas fa-search"></i> Search</button>
          </form>
        
        <router-link class="navbar-brand" to="/register"><i class="fas fa-sign-in-alt"></i>  Inscription</router-link>
        <router-link class="navbar-brand" to="/regimes"><i class="fas fa-sign-in-alt"></i> Régimes </router-link>

              <ul class="navbar-nav">

      <li class="nav-item">
        <span class="nav-link"><i class="fas fa-hourglass-half"></i>  {{ new Date() | moment("dddd, MMMM Do YYYY, h:mm:ss a")  }}</span>
      </li>  
              </ul>
      </nav>
      <router-view></router-view>

      <br><br><br>
 <!-------------ICI les details pour chaque salle-->
 <Section id="sec1" data-text="Section">
  <div id="salle" class="container">
    <h1 style="text-align: center;">L'état actuel dans la salle : </h1>
    <br>
    <br>
    <div class="row">
      <div class="col-4">
        <div class="form-group card border-primary">
          <div class="card-header">
            <h5>LED</h5>
          </div>
          <div class="card-body">
            <ul class="list-group">
              <li
                class="list-group-item d-flex justify-content-between align-items-center"
              >
                Etat :
                <span class="badge badge-primary badge-pill"
                  ><span id="usersB">Eteint</span></span
                >
              </li>
            </ul>
          </div>
        </div>
      </div>
      <div class="col-4">
        <div class="form-group card border-success">
          <div class="card-header">
            <h5>PhotoResistor</h5>
          </div>
          <div class="card-body">
            <ul class="list-group">
              <li
                class="list-group-item d-flex justify-content-between align-items-center"
              >
                Valeur :
                <span class="badge badge-success badge-pill"
                  ><span id="usersB">300</span></span
                >
              </li>
            </ul>
          </div>
        </div>
      </div>
      <div class="col-4">
        <div class="form-group card border-danger">
          <div class="card-header">
            <h5>Température</h5>
          </div>
          <div class="card-body">
            <ul class="list-group">
              <li
                class="list-group-item d-flex justify-content-between align-items-center"
              >
                Valeur :
                <span class="badge badge-danger badge-pill"
                  ><span id="usersB">°</span></span
                >
              </li>
            </ul>
          </div>
        </div>
      </div>
    </div>
    <div class="row">
      <div class="col-2">
        <button style="color: green;" class="ledOn">Allumer</button>
      </div>
      <div  class="col-2">
        <button style="color: red;" class="ledOff">Etteindre</button>
      </div>
    </div>
  </div>

 </Section>

  
  <!-------------ICI le dashboard------------->
  <Section id="sec2" data-text="Section">
    <h1 style="max-width: 600px; margin: 0 auto;" align ="center">Dashbord of Lucioles</h1>

    <br>
    <div id= "menu">
        <div id="container1" style="max-width: 1000px; height: 350px; margin: 0 auto"></div>
        <div id="container2" style="max-width: 1000px; height: 350px; margin: 0 auto"></div>
        <div id="mydiv"></div>
    </div>
  </Section>
        <br><br><br><br><br><br><br><br><br><br><br><br>

  <Section id="sec3" data-text="Section">
    <h4 style="text-align: center; font-weight: bold;">les ampoules</h4>
    <table class="table table-bordered table-striped">
      <thead class="thead-dark">
        <tr>
          <th scope="col">#</th>
          <th scope="col">Salle </th>
          <th scope="col"> <i class="fas fa-lightbulb"></i>  Allumée</th>
          <th scope="col"> <i class="far fa-lightbulb"></i>  Eteinte</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <th scope="row">1</th>
          <td>C3</td>
          <td></td>
          <td></td>
        </tr>
        <tr>
          <th scope="row">2</th>
          <td>C4</td>
          <td></td>
          <td></td>
        </tr>
        <tr>
          <th scope="row">3</th>
          <td>C5</td>
          <td></td>
          <td></td>
        </tr>
        <tr>
          <th scope="row">4</th>
          <td>TD12</td>
          <td></td>
          <td></td>
        </tr>

        <tr>
          <th scope="row">5</th>
          <td>TD13</td>
          <td></td>
          <td></td>
        </tr>
      </tbody>
    </table>
  </Section>

  <Section id="sec4" data-text="Section">
    <h4 style="text-align: center; font-weight: bold;"> Alerte</h4>
    <div class="card text-white bg-danger" style="max-width: 25rem;">
      <div class="card-header">Alerte</div>
      <div class="card-body">
        <h5 class="card-title">Ici on afiche les alertes</h5>
        <p class="card-text">par exemple une led allumée toute la nuite : entre [00:00, 07:00]</p>
      </div>
    </div>
  </Section>
    </div>
</body>
</html>
</template>

<script>
//import Register from "./components/Register.vue";

export default {
  name: 'App',
  components: {
    //Register
  }
}
</script>

<style>
body{
    margin: 0;
    padding: 0;
    background-color: rgba(0, 0, 0, 0.1);
}

section{
    justify-content: center;
    align-items: center;
}


#sec1{
    height: 50vh;
}
#sec2{
    height: 100vh;
}
#sec3{
    height: 80vh;
}
#sec4{
    height: 80vh;
}
</style>