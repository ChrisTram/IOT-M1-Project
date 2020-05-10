<template>
  <div class="registerForm">
<form
  id="app"
  @submit="checkForm"
  action="TODO"
  method="post"
  novalidate="true"
>

  <p v-if="errors.length">
    <b>Please correct the following error(s):</b>
    <ul>
      <li v-for="error in errors" :key="error">>{{ error }}</li>
    </ul>
  </p>

  <p>
    <label for="email">Email</label>
    <input
      id="email"
      v-model="email"
      type="email"
      name="email"
    >
  </p>

  <p>
    <input
      type="submit"
      value="Submit"
    >
  </p>
</form>
  </div>
</template>

<script>
export default {
  name: 'Register',
    data: function() {
      return {
    errors: [],
    email: null,
      }
  },
  methods: {
    checkForm: function (e) {
      this.errors = [];
      if (!this.email) {
        this.errors.push('Email required.');
      }
      if (!this.errors.length) {
        return true;
      }

      e.preventDefault();
    },
  },
  props: {
    msg: String
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h3 {
  margin: 40px 0 0;
}
.registerForm{
    text-align:center;
}
ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
a {
  color: #42b983;
}
</style>
