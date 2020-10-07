import Vue from 'vue';
import App from './App.vue';
import Petitions from './Petitions.vue';
import Petition from './Petition.vue';
import New_Petition from './New_Petition.vue';
import Edit_Petition from './Edit_Petition.vue';
import My_Petitions from './My_Petitions.vue';
import Profile from './Profile.vue';
import Edit_Profile from './Edit_Profile.vue';

import VueRouter from 'vue-router';
Vue.use(VueRouter);

import axios from 'axios';
import VueAxios from 'vue-axios';
Vue.use(VueAxios, axios);

const routes = [
  {
    path: "/",
    name: "petitions",
    component: Petitions
  },
  {
    path: "/petition/:petitionId",
    name: "petition",
    component: Petition
  },
  {
    path: "/new-petition",
    name: "new-petition",
    component: New_Petition
  },
  {
    path: "/edit-petition/:petitionId",
    name: "edit-petition",
    component: Edit_Petition
  },
  {
    path: "/my-petitions",
    name: "my-petitions",
    component: My_Petitions
  },
  {
    path: "/profile",
    name: "profile",
    component: Profile
  },
  {
    path: "/edit-profile",
    name: "edit-profile",
    component: Edit_Profile
  }

];

const router = new VueRouter({
  routes: routes,
  mode: 'history'
});

window.App = new Vue({
  el: '#app',
  router: router,
  render: h => h(App)
})