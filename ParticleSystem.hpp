
#include <SFML/Graphics.hpp>
//#include "saucergame.hpp"

namespace Primus {

class ParticleSystem : public sf::Drawable, public sf::Transformable//, public Thing
{
public:

    ParticleSystem(unsigned int count) :
    m_particles(count),
		m_vertices(sf::Triangles, count *3),
    m_lifetime(sf::seconds(3)),
    m_emitter(0, 0)
    {
	
    }

    void setEmitter(sf::Vector2f position)
    {
         m_emitter = position;
		 		      for (std::size_t i = 0; i < m_particles.size(); ++i)
				  resetParticle(i);

    }

    void update(sf::Time elapsed)
    {
        for (std::size_t i = 0; i < m_particles.size(); ++i)
        {
            // update the particle lifetime
            Particle& p = m_particles[i];
            p.lifetime -= elapsed;

            // if the particle is dead, respawn it
//            if (p.lifetime <= sf::Time::Zero)
//                resetParticle(i);

            // update the position of the corresponding vertex
            m_vertices[3*i+0].position += p.velocity * elapsed.asSeconds();
            m_vertices[3*i+1].position += p.velocity * elapsed.asSeconds();
            m_vertices[3*i+2].position += p.velocity * elapsed.asSeconds();

            // update the alpha (transparency) of the particle according to its lifetime
            float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
            m_vertices[3*i+0].color.a = static_cast<sf::Uint8>(ratio * 255);
            m_vertices[3*i+1].color.a = static_cast<sf::Uint8>(ratio * 255);
            m_vertices[3*i+2].color.a = static_cast<sf::Uint8>(ratio * 255);

			if(p.lifetime <= sf::Time::Zero)
			{

				m_vertices[3*i+0].color = sf::Color::Transparent;
			m_vertices[3*i+1].color = sf::Color::Transparent;
			m_vertices[3*i+2].color = sf::Color::Transparent;
			/*
				m_vertices[3*i+0].color.a = static_cast<sf::Uint8>(255);
				m_vertices[3*i+1].color.a = static_cast<sf::Uint8>(255);
				m_vertices[3*i+2].color.a = static_cast<sf::Uint8>(255);*/
			}else{
			
//#if 0
			m_vertices[3*i+0].color = sf::Color::Red;
			m_vertices[3*i+1].color = sf::Color::Red;
			m_vertices[3*i+2].color = sf::Color::Red;
//#endif
			}
		}
    }

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // our particles don't use a texture
        states.texture = NULL;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

private:

    struct Particle
    {
        sf::Vector2f velocity;
        sf::Time lifetime;
    };

    void resetParticle(std::size_t index)
    {
        // give a random velocity and lifetime to the particle
        float angle = (std::rand() % 360) * 3.14f / 180.f;
        float speed = (std::rand() % 250) + 50.f;
        m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        m_particles[index].lifetime = sf::milliseconds((std::rand() % 1000) + 1000);

        // reset the position of the corresponding vertex
        m_vertices[index*3 + 0].position = m_emitter;
		m_vertices[index*3 + 1].position = m_emitter + sf::Vector2f(std::rand()%6-3,std::rand()%6-3);//(3,0);
		m_vertices[index*3 + 2].position = m_emitter + sf::Vector2f(std::rand()%6-3,std::rand()%6-3);
    }

    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices;
    sf::Time m_lifetime;
    sf::Vector2f m_emitter;
};
}//namespace