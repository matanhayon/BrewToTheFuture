package ema.brewtothefuture.db.model;

import jakarta.persistence.*;

import java.util.List;

@Entity
@Table(name = "brew")
public class BrewDB {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private long id;
    @ManyToOne
    @JoinColumn(name = "recipe_id")
    private RecipeDB recipe;
    private String user_id;
    private long   start_time = System.currentTimeMillis();
    @OneToMany(mappedBy = "brew")
    private List<BrewingReportDB> reports;

    public void setRecipe(RecipeDB recipe) {
        this.recipe = recipe;
    }

    public void setUserId(String userId) {
        this.user_id = userId;
    }

    public long getId() { return id; }
}
