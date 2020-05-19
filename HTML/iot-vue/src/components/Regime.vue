<template>
  <div class="Regime">
    <h1>Régime {{id}}</h1>
    <b-form @submit="onSubmit" @reset="onReset" v-if="show">
      <b-form-group
        id="input-group-1"
        label="Seuil température:"
        label-for="input-1"
        description="En degrés"
      >
        <b-form-input
          id="input-1"
          v-model="form.seuilTemp"
          type="number"
          required
          placeholder="Enter Degrees"
        ></b-form-input>
      </b-form-group>

      <b-form-group
        id="input-group-2"
        label="Seuil Luminosité:"
        label-for="input-2"
        description="En lumins"
      >
        <b-form-input
          id="input-2"
          v-model="form.seuilLumin"
          type="number"
          required
          placeholder="Enter Max Luminosity"
        ></b-form-input>
      </b-form-group>

      <b-form-group
        id="input-group-3"
        label="Durée de mise en veille:"
        label-for="input-3"
        description="En secondes"
      >
        <b-form-input
          id="input-3"
          v-model="form.sleepTime"
          type="number"
          required
          placeholder="Enter Starting time"
        ></b-form-input>
      </b-form-group>

      <b-form-group
        id="input-group-4"
        label="Début du régime (heure et minute):"
        label-for="input-4"
      >
        <b-form-input
          id="input-3"
          v-model="form.beginingRegime"
          type="time"
          required
          placeholder="Enter End Time"
        ></b-form-input>
      </b-form-group>

      <b-form-group id="input-group-5" label="Fin du régime (heure et minute):" label-for="input-5">
        <b-form-input
          id="input-3"
          v-model="form.endRegime"
          type="time"
          required
          placeholder="Enter Sleep Time"
        ></b-form-input>
      </b-form-group>

      <b-button type="submit" variant="primary">Submit</b-button>
      <b-button type="reset" variant="danger">Reset</b-button>
    </b-form>
    <b-card class="mt-3" header="Form Data Result">
      <pre class="m-0">{{ form }}</pre>
    </b-card>
  </div>
</template>

<script>
export default {
  name: "Regime",
  data() {
    return {
      form: {
        seuilTemp: null,
        seuilLumin: null,
        sleepTime: null,
        beginingRegime: null,
        endRegime: null
      },
      show: true
    };
  },
  methods: {
    onSubmit(evt) {
      evt.preventDefault();
      //alert(JSON.stringify(this.form))
      const formData = new FormData();
      this.form.id = this.id;
      formData.append("data", JSON.stringify(this.form));
      console.log(this.form);
      console.log(formData);
      // Submit the form data
      this.$axios
        .post("http://localhost:3000/sendRegime", this.form)
        .then(response => {
          console.log("Submit Success");
          return response;
        })
        .catch(e => {
          console.log("Submit Fail");
          return e;
        });
    },
    onReset(evt) {
      evt.preventDefault();
      // Reset our form values
      this.form.seuilTemp = null;
      this.form.seuilLumin = null;
      this.form.sleepTime = null;
      this.form.beginingRegime = null;
      this.form.endRegime = null;
    }
  },
  props: {
    id: String
  }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.Regime {
  width: 40%;
  margin: 5%;
  text-align: center;
  display: inline-block;
}
</style>
