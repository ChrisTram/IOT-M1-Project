import Vue from 'vue'
import App from './App.vue'
import { BootstrapVue, IconsPlugin } from 'bootstrap-vue'
import LoadScript from 'vue-plugin-load-script';
import VueRouter from "vue-router";

import Register from "./components/Register.vue";
import Regimes from "./components/Regimes.vue";


Vue.use(LoadScript);
// Install BootstrapVue
Vue.use(BootstrapVue)
// Optionally install the BootstrapVue icon components plugin
Vue.use(IconsPlugin)
Vue.config.productionTip = false
Vue.loadScript("https://kit.fontawesome.com/f773ed4f42.js")
Vue.loadScript("https://code.highcharts.com/highcharts.js")
Vue.loadScript("https://code.highcharts.com/modules/exporting.js")
Vue.loadScript("https://code.highcharts.com/modules/export-data.js")

Vue.loadScript("https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js").then(() => {
  console.log("Jquery Loaded")
})

Vue.loadScript("https://cdn.jsdelivr.net/npm/vue/dist/vue.js")

Vue.loadScript("/js/ui_lucioles.js").then(() => {
  console.log("ui_lucioles.js Loaded")
})

Vue.use(require('vue-moment'));
Vue.use(VueRouter);

const router = new VueRouter({
routes: [
  {
    path: "/register",
    name: "register",
    component: Register
  },
  {
    path: "/regimes",
    name: "regime",
    component: Regimes
  }

],
mode: "history"
});

new Vue({
  router,
  render: h => h(App),
}).$mount('#app')